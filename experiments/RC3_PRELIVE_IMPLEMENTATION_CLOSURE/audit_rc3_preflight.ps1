param(
    [Parameter(Mandatory=$false)]
    [string]$Package = ""
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

$ExpectedName = "CSGOVR_ULTIMATE_SAFE161_NATIVE_AIM_RC3_3HOOK_GUARDFIX_ONECLICK.zip"
$ExpectedHash = "B988DEB8DEE490CFB6964ADA004DF69F454AF48A689BDD60E2A67CCE5B72C2FB"
$MaxCopiedBytes = 12MB
$ContextLines = 7

function Resolve-Rc3Package([string]$Requested) {
    if ($Requested) {
        if (Test-Path -LiteralPath $Requested -PathType Leaf) {
            return (Resolve-Path -LiteralPath $Requested).Path
        }
        throw "Package argument does not exist: $Requested"
    }

    $candidates = New-Object System.Collections.Generic.List[string]
    $roots = @(
        (Get-Location).Path,
        $PSScriptRoot,
        (Join-Path $env:USERPROFILE "Downloads"),
        (Join-Path $env:USERPROFILE "Desktop")
    ) | Select-Object -Unique

    foreach ($root in $roots) {
        if (-not $root) { continue }
        $candidate = Join-Path $root $ExpectedName
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            $candidates.Add((Resolve-Path -LiteralPath $candidate).Path)
        }
    }

    $unique = @($candidates | Select-Object -Unique)
    if ($unique.Count -eq 1) { return $unique[0] }
    if ($unique.Count -gt 1) {
        throw "Multiple exact RC3 packages found. Drag the intended ZIP onto 00_RUN_RC3_PRELIVE_COLD.bat or pass its path explicitly.`n$($unique -join "`n")"
    }

    throw "Exact RC3 ZIP not found in current folder, script folder, Downloads, or Desktop. Drag the ZIP onto 00_RUN_RC3_PRELIVE_COLD.bat."
}

function Safe-ReadText([System.IO.FileInfo]$File) {
    try {
        return [System.IO.File]::ReadAllText($File.FullName)
    } catch {
        return $null
    }
}

function Get-RelativePathCompat([string]$Root, [string]$FullPath) {
    $rootFull = [System.IO.Path]::GetFullPath($Root)
    if (-not $rootFull.EndsWith([System.IO.Path]::DirectorySeparatorChar)) {
        $rootFull += [System.IO.Path]::DirectorySeparatorChar
    }
    $full = [System.IO.Path]::GetFullPath($FullPath)
    if ($full.StartsWith($rootFull, [System.StringComparison]::OrdinalIgnoreCase)) {
        return $full.Substring($rootFull.Length)
    }
    return [System.IO.Path]::GetFileName($full)
}

function Write-ContextHits {
    param(
        [string]$Id,
        [string[]]$Patterns,
        [System.IO.FileInfo[]]$Files,
        [string]$OutFile,
        [System.Collections.Generic.HashSet[string]]$RelevantSet
    )

    $hitCount = 0
    $fileSet = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)
    $writer = New-Object System.IO.StreamWriter($OutFile, $false, (New-Object System.Text.UTF8Encoding($false)))
    try {
        $writer.WriteLine("GROUP=$Id")
        $writer.WriteLine("PATTERNS=$($Patterns -join ' || ')")
        $writer.WriteLine("")

        foreach ($file in $Files) {
            $text = Safe-ReadText $file
            if ($null -eq $text) { continue }
            $lines = $text -split "`r?`n"
            $matchedLines = New-Object System.Collections.Generic.List[int]
            for ($i=0; $i -lt $lines.Count; $i++) {
                $matched = $false
                foreach ($pat in $Patterns) {
                    if ($lines[$i] -match $pat) { $matched = $true; break }
                }
                if ($matched) { $matchedLines.Add($i) }
            }
            if ($matchedLines.Count -eq 0) { continue }

            $rel = Get-RelativePathCompat $script:ExtractRoot $file.FullName
            [void]$fileSet.Add($rel)
            [void]$RelevantSet.Add($file.FullName)
            $writer.WriteLine("===== FILE: $rel =====")

            $lastEnd = -2
            foreach ($idx in $matchedLines) {
                $start = [Math]::Max(0, $idx - $ContextLines)
                $end = [Math]::Min($lines.Count - 1, $idx + $ContextLines)
                if ($start -le ($lastEnd + 1)) { $start = $lastEnd + 1 }
                if ($start -gt $end) { continue }
                $writer.WriteLine("--- match line $($idx+1), context $($start+1)-$($end+1) ---")
                for ($j=$start; $j -le $end; $j++) {
                    $marker = if ($j -eq $idx) { ">" } else { " " }
                    $writer.WriteLine(("{0}{1,6}: {2}" -f $marker, ($j+1), $lines[$j]))
                }
                $writer.WriteLine("")
                $lastEnd = $end
                $hitCount++
            }
        }
    } finally {
        $writer.Dispose()
    }

    return [PSCustomObject]@{
        Id = $Id
        Hits = $hitCount
        Files = $fileSet.Count
    }
}

$PackagePath = Resolve-Rc3Package $Package
$ActualHash = (Get-FileHash -LiteralPath $PackagePath -Algorithm SHA256).Hash.ToUpperInvariant()
if ($ActualHash -ne $ExpectedHash) {
    Write-Host "[FAIL] RC3 package SHA256 mismatch." -ForegroundColor Red
    Write-Host "EXPECTED=$ExpectedHash"
    Write-Host "ACTUAL=$ActualHash"
    Write-Host "PATH=$PackagePath"
    exit 10
}

$stamp = Get-Date -Format "yyyyMMdd_HHmmss"
$parent = Split-Path -Parent $PackagePath
$ResultRoot = Join-Path $parent ("CSGOVR_RC3_PRELIVE_COLD_RESULT_" + $stamp)
$script:ExtractRoot = Join-Path $env:TEMP ("CSGOVR_RC3_PRELIVE_" + [Guid]::NewGuid().ToString("N"))
$HitsRoot = Join-Path $ResultRoot "HITS"
$SourceRoot = Join-Path $ResultRoot "RELEVANT_SOURCE"

New-Item -ItemType Directory -Force -Path $ResultRoot,$HitsRoot,$SourceRoot,$script:ExtractRoot | Out-Null

try {
    Expand-Archive -LiteralPath $PackagePath -DestinationPath $script:ExtractRoot -Force

    $extensions = @('.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.inl','.ipp','.ps1','.bat','.cmd','.py','.md','.txt','.json','.ini','.cfg','.cmake')
    $textFiles = @(Get-ChildItem -LiteralPath $script:ExtractRoot -Recurse -File | Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() })
    $sourceFiles = @($textFiles | Where-Object { @('.c','.cc','.cpp','.cxx','.h','.hh','.hpp','.inl','.ipp') -contains $_.Extension.ToLowerInvariant() })

    @(
        "PACKAGE=$PackagePath",
        "EXPECTED_NAME=$ExpectedName",
        "EXPECTED_SHA256=$ExpectedHash",
        "ACTUAL_SHA256=$ActualHash",
        "SHA256_MATCH=PASS",
        "TEXT_FILES=$($textFiles.Count)",
        "SOURCE_FILES=$($sourceFiles.Count)",
        "GAME_LAUNCHED=NO",
        "BUILD_PERFORMED=NO",
        "INJECTION_PERFORMED=NO",
        "SOURCE_MUTATION=NO"
    ) | Set-Content -LiteralPath (Join-Path $ResultRoot "PACKAGE_ID.txt") -Encoding UTF8

    $allList = foreach ($f in (Get-ChildItem -LiteralPath $script:ExtractRoot -Recurse -File | Sort-Object FullName)) {
        $rel = Get-RelativePathCompat $script:ExtractRoot $f.FullName
        "{0}`t{1}" -f $f.Length,$rel
    }
    $allList | Set-Content -LiteralPath (Join-Path $ResultRoot "PACKAGE_FILELIST.tsv") -Encoding UTF8

    $groups = @(
        [PSCustomObject]@{ Id='G01_TOPOLOGY'; Patterns=@('CheckSplitScreenMimic','IsInputDeviceAPointer','MH_CreateHook','MH_QueueEnableHook','MH_EnableHook','MH_ApplyQueued') },
        [PSCustomObject]@{ Id='G02_RING_IDENTITY'; Patterns=@('sessionEpoch','session_epoch','command_number','commandNumber','tick_count','tickCount','aimBits','aim_bits','localPlayerIndex','local_player_index','sampleId','sample_id','poseSeq','pose_seq') },
        [PSCustomObject]@{ Id='G03_RING_PUBLICATION_ORDER'; Patterns=@('MemoryBarrier','Interlocked','atomic','sequence','seq','publish','valid','generation','commit') },
        [PSCustomObject]@{ Id='G04_SESSION_INVALIDATION'; Patterns=@('sessionEpoch','epoch','disconnect','reconnect','LevelShutdown','LevelInit','map.?change','server.?change','reset','invalidate','clear.*ring','memset.*ring') },
        [PSCustomObject]@{ Id='G05_FLOAT_CANONICALIZATION'; Patterns=@('_finite','isfinite','isnan','isinf','NaN','Inf','signed.?zero','canonical.*zero','-0\.0','==\s*0\.0f','FloatBits','float.?bits') },
        [PSCustomObject]@{ Id='G06_COMMAND_STAMP_POLICY'; Patterns=@('IN_ATTACK','buttons','CheckSplitScreenMimic','aimdirection','aimDirection','command_number') },
        [PSCustomObject]@{ Id='G07_STOCK_REJECTS'; Patterns=@('wrong.?caller','wrong.?player','remote','bot','reject','mismatch','return.*original','Original.*IsInputDeviceAPointer','stock','localPlayerIndex','currentCmd') },
        [PSCustomObject]@{ Id='G08_HOOK_TRANSACTION'; Patterns=@('bundleReady','bundle_ready','ready','MH_CreateHook','MH_QueueEnableHook','MH_EnableHook','MH_ApplyQueued','MH_DisableHook','QueueDisable','rollback','FAILED_PERMANENT','InterlockedExchange') },
        [PSCustomObject]@{ Id='G09_RELOCATION_GUARD'; Patterns=@('005BC67A','5BC67A','00A968E0','A968E0','actualServerBase','serverBase','moffs','relocat','opcode.*A1','\bA1\b') },
        [PSCustomObject]@{ Id='G10_FORBIDDEN_PATHS'; Patterns=@('RunCommand','thread_local','__declspec\s*\(\s*thread','TLS','FX_FireBullets','DrawModel','PVS','muzzle','attachment') },
        [PSCustomObject]@{ Id='G11_SAFE161_LINEAGE'; Patterns=@('SAFE161','WEAPON_SAFE_POSTCALC_AIM','xr_host','Quest','matrix','manifest','SHA256','protected') },
        [PSCustomObject]@{ Id='G12_RESULT_TELEMETRY'; Patterns=@('NATIVE_AIM_EXACT_GUARD','NATIVE_AIM_3HOOK_ACTIVE','NATIVE_AIM_PUBLISH','NATIVE_AIM_CLIENT_GATE','NATIVE_AIM_SERVER_GATE','RunCommandHook=ABSENT','TLS=ABSENT') }
    )

    $relevant = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)
    $rows = New-Object System.Collections.Generic.List[object]

    foreach ($g in $groups) {
        $out = Join-Path $HitsRoot ($g.Id + ".txt")
        $row = Write-ContextHits -Id $g.Id -Patterns $g.Patterns -Files $textFiles -OutFile $out -RelevantSet $relevant
        $rows.Add($row)
    }

    "GROUP`tCONTEXT_HITS`tFILES_WITH_HITS" | Set-Content -LiteralPath (Join-Path $ResultRoot "GROUP_COUNTS.tsv") -Encoding UTF8
    foreach ($r in $rows) {
        ("{0}`t{1}`t{2}" -f $r.Id,$r.Hits,$r.Files) | Add-Content -LiteralPath (Join-Path $ResultRoot "GROUP_COUNTS.tsv") -Encoding UTF8
    }

    $copied = 0L
    $copiedCount = 0
    $skipped = New-Object System.Collections.Generic.List[string]
    foreach ($full in @($relevant | Sort-Object)) {
        $f = Get-Item -LiteralPath $full
        $rel = Get-RelativePathCompat $script:ExtractRoot $f.FullName
        if (($copied + $f.Length) -gt $MaxCopiedBytes) {
            $skipped.Add(("SIZE_CAP`t{0}`t{1}" -f $f.Length,$rel))
            continue
        }
        $dest = Join-Path $SourceRoot $rel
        $destDir = Split-Path -Parent $dest
        New-Item -ItemType Directory -Force -Path $destDir | Out-Null
        Copy-Item -LiteralPath $f.FullName -Destination $dest -Force
        $copied += $f.Length
        $copiedCount++
    }

    $skipped | Set-Content -LiteralPath (Join-Path $ResultRoot "RELEVANT_SOURCE_SKIPPED.tsv") -Encoding UTF8

    "SHA256`tBYTES`tPATH" | Set-Content -LiteralPath (Join-Path $ResultRoot "RELEVANT_SOURCE_SHA256.tsv") -Encoding UTF8
    foreach ($f in @(Get-ChildItem -LiteralPath $SourceRoot -Recurse -File | Sort-Object FullName)) {
        $rel = Get-RelativePathCompat $SourceRoot $f.FullName
        $sha = (Get-FileHash -LiteralPath $f.FullName -Algorithm SHA256).Hash
        ("{0}`t{1}`t{2}" -f $sha,$f.Length,$rel) | Add-Content -LiteralPath (Join-Path $ResultRoot "RELEVANT_SOURCE_SHA256.tsv") -Encoding UTF8
    }

    $groupMap = @{}
    foreach ($r in $rows) { $groupMap[$r.Id] = $r }
    $missingGroups = @()
    foreach ($required in @('G01_TOPOLOGY','G02_RING_IDENTITY','G04_SESSION_INVALIDATION','G05_FLOAT_CANONICALIZATION','G07_STOCK_REJECTS','G08_HOOK_TRANSACTION','G09_RELOCATION_GUARD')) {
        if (-not $groupMap.ContainsKey($required) -or $groupMap[$required].Hits -eq 0) { $missingGroups += $required }
    }

    @(
        "RC3_PRELIVE_COLD_CAPTURE=COMPLETE",
        "PACKAGE_SHA_MATCH=PASS",
        "SOURCE_FILES=$($sourceFiles.Count)",
        "RELEVANT_SOURCE_FILES_COPIED=$copiedCount",
        "RELEVANT_SOURCE_BYTES=$copied",
        "AUTOMATIC_SEMANTIC_VERDICT=WITHHELD",
        "WHY=token presence is evidence location, not proof of control-flow semantics",
        "MISSING_REQUIRED_HIT_GROUPS=$($missingGroups -join ',')",
        "NEXT=review HITS and RELEVANT_SOURCE against the ten canonical RC3 implementation invariants in docs/SYNC.md",
        "LIVE_AUTHORIZED_BY_THIS_SCRIPT=NO"
    ) | Set-Content -LiteralPath (Join-Path $ResultRoot "SUMMARY.txt") -Encoding UTF8

    @(
        "This result is a read-only exact-package evidence capture.",
        "It deliberately does not promote RC3 from token matches.",
        "Review PACKAGE_ID.txt, GROUP_COUNTS.tsv, HITS/*.txt and RELEVANT_SOURCE/.",
        "A group with zero hits is a strong missing-evidence signal, not automatically proof that the implementation is absent if naming differs.",
        "A group with hits is only a locator. The source context must prove the required semantics.",
        "No game, injector, compiler, DLL installer or patcher is launched by this audit."
    ) | Set-Content -LiteralPath (Join-Path $ResultRoot "READ_ME_FIRST.txt") -Encoding UTF8

    $zipPath = $ResultRoot + ".zip"
    if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }
    Compress-Archive -Path (Join-Path $ResultRoot '*') -DestinationPath $zipPath -CompressionLevel Optimal

    Write-Host ""
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host " CSGOVR RC3 PRE-LIVE COLD CAPTURE COMPLETE" -ForegroundColor Cyan
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "PACKAGE SHA256: PASS" -ForegroundColor Green
    Write-Host "RESULT ZIP: $zipPath" -ForegroundColor Green
    Write-Host "SEMANTIC VERDICT: WITHHELD until source review" -ForegroundColor Yellow
    if ($missingGroups.Count -gt 0) {
        Write-Host "NO-HIT GROUPS: $($missingGroups -join ', ')" -ForegroundColor Yellow
    }
    exit 0
}
catch {
    Write-Host "[FAIL] $($_.Exception.Message)" -ForegroundColor Red
    try {
        $_ | Out-String | Set-Content -LiteralPath (Join-Path $ResultRoot "FATAL_ERROR.txt") -Encoding UTF8
    } catch {}
    exit 20
}
finally {
    if ($script:ExtractRoot -and (Test-Path -LiteralPath $script:ExtractRoot)) {
        Remove-Item -LiteralPath $script:ExtractRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}
