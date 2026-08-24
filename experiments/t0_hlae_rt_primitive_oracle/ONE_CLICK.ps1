$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$ToolsRoot = 'C:\GOG Games\VR Research Tools'
$ExpectedAfx = '5B9D13AE9ABFFEEF80851E10861302162A47D1B43D8483B4A8FCEC7EAA19B035'
$ExpectedTimestamp = 0x6A58EC87
$ExpectedImageSize = 0x0023F000

function Banner([string]$Text) {
    Write-Host ''
    Write-Host '=============================================================================='
    Write-Host (' ' + $Text)
    Write-Host '=============================================================================='
}

function Find-ExactAfx {
    $roots = New-Object System.Collections.Generic.List[string]
    foreach ($p in @(
        (Join-Path $ToolsRoot 'HLAE'),
        'C:\Program Files\HLAE',
        'C:\Program Files (x86)\HLAE',
        'C:\Program Files (x86)\Steam\steamapps\common\csgo legacy'
    )) {
        if (Test-Path -LiteralPath $p) { $roots.Add($p) }
    }
    $seen = @{}
    foreach ($r in $roots) {
        foreach ($f in (Get-ChildItem -LiteralPath $r -Filter 'AfxHookSource.dll' -File -Recurse -ErrorAction SilentlyContinue)) {
            if ($seen.ContainsKey($f.FullName)) { continue }
            $seen[$f.FullName] = $true
            try {
                $h = (Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash.ToUpperInvariant()
                Write-Host ('[CANDIDATE] ' + $h + '  ' + $f.FullName)
                if ($h -eq $ExpectedAfx) { return $f.FullName }
            } catch {}
        }
    }
    throw 'Exact HLAE 2.191.1 AfxHookSource.dll not found.'
}

function Find-GhidraHeadless {
    $x = Get-ChildItem -LiteralPath $ToolsRoot -Filter 'analyzeHeadless.bat' -File -Recurse -ErrorAction SilentlyContinue |
        Sort-Object FullName | Select-Object -ExpandProperty FullName -First 1
    if (-not $x) { throw 'Ghidra analyzeHeadless.bat not found under shared tools root.' }
    return $x
}

function Find-JdkHome {
    foreach ($javac in (Get-ChildItem -LiteralPath $ToolsRoot -Filter 'javac.exe' -File -Recurse -ErrorAction SilentlyContinue | Sort-Object FullName)) {
        $home = Split-Path -Parent (Split-Path -Parent $javac.FullName)
        if (Test-Path -LiteralPath (Join-Path $home 'bin\java.exe')) { return $home }
    }
    return $null
}

function Get-ScanValue([string]$Text, [string]$Key) {
    $m = [regex]::Match($Text, '(?m)^' + [regex]::Escape($Key) + '=(.*)$')
    if ($m.Success) { return $m.Groups[1].Value.Trim() }
    return 'MISSING'
}

Banner 'CSGOVR T0 COLD HLAE RT PRIMITIVE ORACLE v1.1.0'
Write-Host 'READ-ONLY STATIC ANALYSIS. CS:GO/HLAE WILL NOT BE LAUNCHED OR PATCHED.'

Banner '1/6 EXACT INPUT'
$Afx = Find-ExactAfx
$AfxHash = (Get-FileHash -LiteralPath $Afx -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host ('[OK] ' + $Afx)

Banner '2/6 TOOLCHAIN'
$Ghidra = Find-GhidraHeadless
$Jdk = Find-JdkHome
if ($Jdk) {
    $env:JAVA_HOME = $Jdk
    $env:PATH = (Join-Path $Jdk 'bin') + ';' + $env:PATH
    Write-Host ('[OK] JAVA_HOME=' + $Jdk)
} else {
    Write-Host '[INFO] No private JDK override found; Ghidra launcher will use its configured Java.'
}
Write-Host ('[OK] Ghidra=' + $Ghidra)

Banner '3/6 STAGE BYTE-IDENTICAL INPUT'
$Stamp = Get-Date -Format 'yyyyMMdd_HHmmss'
$Stage = Join-Path $Root ('CSGOVR_T0_HLAE_RT_PRIMITIVE_RESULT_' + $Stamp)
$InputDir = Join-Path $Stage '_input'
$ProjectDir = Join-Path $Stage '_ghidra_project'
New-Item -ItemType Directory -Force -Path $InputDir,$ProjectDir | Out-Null
$StagedAfx = Join-Path $InputDir 'AfxHookSource.dll'
Copy-Item -LiteralPath $Afx -Destination $StagedAfx -Force
$StagedHash = (Get-FileHash -LiteralPath $StagedAfx -Algorithm SHA256).Hash.ToUpperInvariant()
if ($StagedHash -ne $ExpectedAfx) { throw 'Staged AfxHookSource.dll hash mismatch.' }
@(
    'EvidenceLabel=STATIC-AUDITED / NOT LIVE-PROVEN',
    ('OracleVersion=1.1.0'),
    ('OriginalPath=' + $Afx),
    ('SHA256=' + $StagedHash),
    ('ExpectedTimestamp=0x{0:X8}' -f $ExpectedTimestamp),
    ('ExpectedSizeOfImage=0x{0:X8}' -f $ExpectedImageSize),
    ('Generated=' + (Get-Date).ToString('o'))
) | Set-Content -LiteralPath (Join-Path $Stage 'INPUT_IDENTITY.txt') -Encoding UTF8

Banner '4/6 GHIDRA HEADLESS SCAN'
$ScriptDir = Join-Path $Root 'scripts'
$Scan = Join-Path $Stage 'HLAE_RT_PRIMITIVE_SCAN.txt'
$ProjectName = 'csgovr_t0_hlae_rt_' + $Stamp
$ghArgs = @(
    $ProjectDir,
    $ProjectName,
    '-import', $StagedAfx,
    '-overwrite',
    '-scriptPath', $ScriptDir,
    '-postScript', 'CsgovrHlaeRtPrimitiveProbe.java', $Scan,
    '-deleteProject'
)
$saved = $ErrorActionPreference
$ErrorActionPreference = 'Continue'
try {
    & $Ghidra @ghArgs 2>&1 | Tee-Object -FilePath (Join-Path $Stage 'GHIDRA_HEADLESS.log')
    $GhidraEc = $LASTEXITCODE
} finally {
    $ErrorActionPreference = $saved
}
if ($GhidraEc -ne 0) { throw ('Ghidra exit code ' + $GhidraEc) }
if (-not (Test-Path -LiteralPath $Scan)) { throw 'Ghidra scan file missing.' }
if (-not (Select-String -LiteralPath $Scan -SimpleMatch 'SCAN_COMPLETE=YES' -Quiet)) { throw 'Ghidra scan incomplete.' }

Banner '5/6 RESULT CONTRACT'
@(
    'MISSING FACT: exact frozen AfxHookSource.dll helper RVAs / binary fingerprints for HLAE-state-aware render-target replacement and GPU completion.',
    'WHY NOT COLD: this experiment IS the cold/static closure; no game runtime is required or authorized.',
    'PASS: source-guided binary fingerprints collapse PushRT/PopRT/GPU-wait to bounded finalists with decompile/caller context sufficient for exact source-semantic review.',
    'FAIL: core roles remain ambiguous or exact binary semantics contradict the pinned source model; no runtime mutation is authorized from this result.',
    'BASELINE MUST HOLD: no CS:GO launch, no HLAE launch, no process injection, no file patching, exact input hash only.'
) | Set-Content -LiteralPath (Join-Path $Stage 'PREFLIGHT.txt') -Encoding UTF8

$scanText = Get-Content -LiteralPath $Scan -Raw
$semantic = Get-ScanValue $scanText 'SemanticVerdict'
$bounded = Get-ScanValue $scanText 'BoundedCoreFinalists'
$push = Get-ScanValue $scanText 'BestPushMemberRVA'
$pop = Get-ScanValue $scanText 'BestPopMemberRVA'
$wait = Get-ScanValue $scanText 'BestGpuWaitMemberRVA'
$resultClass = if ($semantic -eq 'BOUNDED_FINALISTS') { 'PASS_BOUNDED_FINALISTS_PENDING_SOURCE_REVIEW' } else { 'INCONCLUSIVE_STATIC' }

@(
    'CSGOVR T0 HLAE RT SOURCE-GUIDED ROLE SUMMARY',
    'EvidenceLabel=STATIC-AUDITED / NOT LIVE-PROVEN',
    ('SemanticVerdict=' + $semantic),
    ('BoundedCoreFinalists=' + $bounded),
    ('BestPushMemberRVA=' + $push),
    ('BestPopMemberRVA=' + $pop),
    ('BestGpuWaitMemberRVA=' + $wait),
    'PromotionPolicy=HEURISTIC ONLY; compare candidate decompile and thin callers against pinned source before promotion.',
    'RuntimeAuthorization=NO'
) | Set-Content -LiteralPath (Join-Path $Stage 'ROLE_SUMMARY.txt') -Encoding UTF8

@(
    'CSGOVR T0 HLAE RT PRIMITIVE ORACLE',
    'EvidenceLabel=STATIC-AUDITED / NOT LIVE-PROVEN',
    'OracleVersion=1.1.0',
    ('ExactHash=' + $StagedHash),
    ('ScanComplete=' + [bool]($scanText -match 'SCAN_COMPLETE=YES')),
    ('ResultClass=' + $resultClass),
    ('SemanticVerdict=' + $semantic),
    ('BoundedCoreFinalists=' + $bounded),
    ('BestPushMemberRVA=' + $push),
    ('BestPopMemberRVA=' + $pop),
    ('BestGpuWaitMemberRVA=' + $wait),
    'InterpretationPolicy=Source-guided role output narrows candidates but never auto-promotes an RVA to PROVEN.',
    'NoRuntimeResultClaimed=YES'
) | Set-Content -LiteralPath (Join-Path $Stage 'RESULT_SUMMARY.txt') -Encoding UTF8

Banner '6/6 PACKAGE'
$Zip = $Stage + '.zip'
Add-Type -AssemblyName System.IO.Compression.FileSystem
if (Test-Path -LiteralPath $Zip) { Remove-Item -LiteralPath $Zip -Force }
[System.IO.Compression.ZipFile]::CreateFromDirectory($Stage, $Zip, [System.IO.Compression.CompressionLevel]::Optimal, $false)
$ZipHash = (Get-FileHash -LiteralPath $Zip -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host ('[DONE] RESULT=' + $Zip)
Write-Host ('[DONE] SHA256=' + $ZipHash)
Write-Host ('[DONE] SemanticVerdict=' + $semantic)
Write-Host ('[DONE] Push=' + $push + ' Pop=' + $pop + ' Wait=' + $wait)
Write-Host '[RETURN] Send the small RESULT ZIP back to the CSGOVR branch.'
