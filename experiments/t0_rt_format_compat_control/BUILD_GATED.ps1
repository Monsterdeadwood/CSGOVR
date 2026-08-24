$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $Root '..\..')
$ToolsRoot = 'C:\GOG Games\VR Research Tools'
$ConfigPath = Join-Path $Root 'PROMOTED_RVAS.ini'
$ExpectedAfx = '5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035'

function Banner([string]$Text) {
    Write-Host ''
    Write-Host '=============================================================================='
    Write-Host (' ' + $Text)
    Write-Host '=============================================================================='
}

function Read-SimpleIni([string]$Path) {
    $out = @{}
    foreach ($line in Get-Content -LiteralPath $Path) {
        $s = $line.Trim()
        if (-not $s -or $s.StartsWith('#') -or $s.StartsWith(';')) { continue }
        $eq = $s.IndexOf('=')
        if ($eq -le 0) { continue }
        $k = $s.Substring(0,$eq).Trim()
        $v = $s.Substring($eq+1).Trim()
        $out[$k] = $v
    }
    return $out
}

function Require-Key($Cfg, [string]$Key) {
    if (-not $Cfg.ContainsKey($Key) -or [string]::IsNullOrWhiteSpace([string]$Cfg[$Key])) {
        throw ('Missing promotion key: ' + $Key)
    }
    return [string]$Cfg[$Key]
}

function Find-Zig016 {
    $candidates = @()
    foreach ($base in @($ToolsRoot, [string]$RepoRoot)) {
        if (Test-Path -LiteralPath $base) {
            $candidates += Get-ChildItem -LiteralPath $base -Filter 'zig.exe' -File -Recurse -ErrorAction SilentlyContinue |
                Sort-Object FullName
        }
    }
    foreach ($z in $candidates) {
        $v = (& $z.FullName version 2>$null | Out-String).Trim()
        if ($v -eq '0.16.0') { return $z.FullName }
    }
    throw 'Zig 0.16.0 not found.'
}

function Find-MinHookRoot {
    foreach ($base in @([string]$RepoRoot, $ToolsRoot)) {
        if (-not (Test-Path -LiteralPath $base)) { continue }
        foreach ($h in (Get-ChildItem -LiteralPath $base -Filter 'MinHook.h' -File -Recurse -ErrorAction SilentlyContinue | Sort-Object FullName)) {
            $inc = Split-Path -Parent $h.FullName
            $candidate = if ((Split-Path -Leaf $inc) -ieq 'include') { Split-Path -Parent $inc } else { $inc }
            if ((Test-Path -LiteralPath (Join-Path $candidate 'src\buffer.c')) -and
                (Test-Path -LiteralPath (Join-Path $candidate 'src\hook.c')) -and
                (Test-Path -LiteralPath (Join-Path $candidate 'src\trampoline.c')) -and
                (Test-Path -LiteralPath (Join-Path $candidate 'src\hde\hde32.c'))) {
                return $candidate
            }
        }
    }
    throw 'MinHook source tree not found.'
}

Banner 'CSGOVR T0 RT FORMAT COMPAT CONTROL - GATED BUILD ONLY'
Write-Host 'This script never launches CS:GO/HLAE and never injects the produced DLL.'

Banner '1/5 PROMOTION GATE'
if (-not (Test-Path -LiteralPath $ConfigPath)) {
    throw 'PROMOTED_RVAS.ini is absent. This is intentional before bounded compatibility LIVE authorization. Build refused.'
}
$cfg = Read-SimpleIni $ConfigPath
$status = Require-Key $cfg 'Status'
if ($status -ne 'PROVEN_SOURCE_MATCH') { throw ('Status must be PROVEN_SOURCE_MATCH, got: ' + $status) }
$afxHash = (Require-Key $cfg 'ExactAfxSHA256').ToUpperInvariant()
if ($afxHash -ne $ExpectedAfx) { throw 'ExactAfxSHA256 does not match frozen HLAE 2.191.1.' }
$coldHash = (Require-Key $cfg 'ColdResultSHA256').ToUpperInvariant()
if ($coldHash -notmatch '^[0-9A-F]{64}$') { throw 'ColdResultSHA256 must be exactly 64 hex characters.' }
$review = Require-Key $cfg 'ReviewerVerdict'
if ($review -ne 'PROVEN_SOURCE_MATCH') { throw 'ReviewerVerdict must be PROVEN_SOURCE_MATCH.' }
$rvaText = Require-Key $cfg 'PushPrimitiveRVA'
if ($rvaText -notmatch '^0x[0-9A-Fa-f]{1,8}$') { throw 'PushPrimitiveRVA must be 0x... hex.' }
$rva = [Convert]::ToUInt32($rvaText.Substring(2),16)
if ($rva -eq 0) { throw 'PushPrimitiveRVA cannot be zero.' }
$sig = (Require-Key $cfg 'PushPrimitiveSigHex').Replace(' ','').Replace('-','').ToUpperInvariant()
if ($sig -notmatch '^[0-9A-F]+$' -or ($sig.Length % 2) -ne 0) { throw 'PushPrimitiveSigHex must be even-length hex.' }
$sigBytes = [int]($sig.Length / 2)
if ($sigBytes -lt 6 -or $sigBytes -gt 64) { throw 'PushPrimitiveSigHex must contain 6..64 bytes.' }
Write-Host ('[OK] promoted Push primitive RVA=' + $rvaText + ' signatureBytes=' + $sigBytes)
Write-Host ('[OK] cold RESULT SHA256=' + $coldHash)

Banner '2/5 TOOLCHAIN'
$zig = Find-Zig016
$mh = Find-MinHookRoot
$mhInclude = if (Test-Path -LiteralPath (Join-Path $mh 'include\MinHook.h')) { Join-Path $mh 'include' } else { $mh }
Write-Host ('[OK] Zig=' + $zig)
Write-Host ('[OK] MinHook=' + $mh)

Banner '3/5 GENERATED PROMOTION HEADER'
$Build = Join-Path $Root '_BUILD_GATED'
if (Test-Path -LiteralPath $Build) { Remove-Item -LiteralPath $Build -Recurse -Force }
New-Item -ItemType Directory -Force -Path $Build | Out-Null
$header = Join-Path $Build 'promoted_rvas.h'
$byteItems = New-Object System.Collections.Generic.List[string]
for ($i=0; $i -lt $sig.Length; $i += 2) { $byteItems.Add(('0x' + $sig.Substring($i,2))) }
@(
    '#pragma once',
    ('#define CSGOVR_PUSH_PRIMITIVE_RVA 0x{0:X8}u' -f $rva),
    ('#define CSGOVR_PROMOTION_COLD_RESULT_SHA256 "' + $coldHash + '"'),
    ('static const unsigned char CSGOVR_PUSH_PRIMITIVE_SIG[] = { ' + ($byteItems -join ', ') + ' };'),
    '#define CSGOVR_PUSH_PRIMITIVE_SIG_LEN ((unsigned int)sizeof(CSGOVR_PUSH_PRIMITIVE_SIG))',
    '',
    '/* Temporary source compatibility aliases. The target is the promoted primitive itself, not an unknown wrapper. */',
    '#define CSGOVR_PUSH_WRAPPER_RVA CSGOVR_PUSH_PRIMITIVE_RVA',
    '#define CSGOVR_PUSH_WRAPPER_SIG CSGOVR_PUSH_PRIMITIVE_SIG',
    '#define CSGOVR_PUSH_WRAPPER_SIG_LEN CSGOVR_PUSH_PRIMITIVE_SIG_LEN'
) | Set-Content -LiteralPath $header -Encoding ASCII
Write-Host ('[OK] generated ' + $header)

Banner '4/5 BUILD DLL ONLY'
$src = Join-Path $Root 'src\compat_probe.c'
if (-not (Test-Path -LiteralPath $src)) { throw 'compat_probe.c missing.' }
$outDll = Join-Path $Build 'csgovr_t0_rt_format_compat_probe.dll'
$args = @(
    'cc','-target','x86-windows-gnu','-std=gnu11','-O2','-shared',
    '-I', $Build,
    '-I', $mhInclude,
    $src,
    (Join-Path $mh 'src\buffer.c'),
    (Join-Path $mh 'src\hook.c'),
    (Join-Path $mh 'src\trampoline.c'),
    (Join-Path $mh 'src\hde\hde32.c'),
    '-ld3d9','-ldxguid','-lkernel32','-lshell32',
    '-o', $outDll
)
& $zig @args
if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $outDll)) { throw ('Zig build failed: ' + $LASTEXITCODE) }
$dllHash = (Get-FileHash -LiteralPath $outDll -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host ('[OK] DLL=' + $outDll)
Write-Host ('[OK] SHA256=' + $dllHash)

Banner '5/5 BUILD CONTRACT'
@(
    'Status=BUILT_NOT_RUN',
    'EvidenceLabel=PREPARED / GATED / NOT LIVE-PROVEN',
    ('ColdResultSHA256=' + $coldHash),
    ('ExactAfxSHA256=' + $afxHash),
    ('PushPrimitiveRVA=' + ('0x{0:X8}' -f $rva)),
    ('PushPrimitiveSigBytes=' + $sigBytes),
    ('ProbeDLLSHA256=' + $dllHash),
    ('Zig=' + $zig),
    ('MinHook=' + $mh),
    'LaunchCommandIncluded=NO',
    'InjectorIncluded=NO',
    'OpenXRIncluded=NO'
) | Set-Content -LiteralPath (Join-Path $Build 'BUILD_INFO.txt') -Encoding UTF8

Write-Host ''
Write-Host '[DONE] GATED BUILD ONLY. No game was launched and no DLL was injected.' -ForegroundColor Green
Write-Host '[STOP] Do not inject until the bounded compatibility LIVE is explicitly authorized.' -ForegroundColor Yellow
