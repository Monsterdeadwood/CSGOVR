[CmdletBinding()]
param([string]$LaunchLog='')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot));$game=[IO.Path]::GetFullPath((Join-Path $root '..'))
$reports=Join-Path $root 'REPORTS';[void](New-Item -ItemType Directory -Path $reports -Force)
$name='NOVA_REPORT_'+[DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_fff')
$temporary=Join-Path $reports ('.collect_'+[Guid]::NewGuid().ToString('N'))
$out=Join-Path $temporary $name;[void](New-Item -ItemType Directory -Path $out -Force)
$utf8=[Text.UTF8Encoding]::new($false)
$index=[ordered]@{schema='nova.discord-report.v1';utc=[DateTime]::UtcNow.ToString('o');scope='LOCAL_WHITELISTED_OWN_SESSION;NO_AUTOMATIC_UPLOAD';privacy='Known installation/home paths replaced; no screenshots, audio, memory dump, network or account inventory';files=@();issues=@()}
$aliases=@(@{from=$root;to='<PACKAGE>'},@{from=$game;to='<GAME>'},@{from=$env:USERPROFILE;to='<USER>'})
function Scrub([string]$text){
 foreach($alias in $aliases){if(-not $alias.from){continue};foreach($variant in @($alias.from,$alias.from.Replace('\','\\'),$alias.from.Replace('\','/'))){$text=[regex]::Replace($text,[regex]::Escape($variant),$alias.to,[Text.RegularExpressions.RegexOptions]::IgnoreCase)}}
 $text=[regex]::Replace($text,'(?i)(userdata[\\/]{1,2})[0-9]+','$1<STEAM_USER>')
 return $text
}
function Copy-Text([string]$source,[string]$relative){
 if(-not (Test-Path -LiteralPath $source -PathType Leaf)){return}
 $limit=3MB;$stream=$null;$truncated=$false
 try{
  $stream=[IO.FileStream]::new($source,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite -bor [IO.FileShare]::Delete)
  $length=$stream.Length;$take=[int][Math]::Min($length,$limit);$start=$length-$take
  if($start -gt 0){$stream.Position=$start;$truncated=$true}
  $bytes=New-Object byte[] $take;$read=0
  while($read -lt $take){$n=$stream.Read($bytes,$read,$take-$read);if(-not $n){break};$read+=$n}
  $text=[Text.Encoding]::UTF8.GetString($bytes,0,$read).TrimStart([char]0xfeff)
  if($relative -eq 'launch.log'){
   # The PowerShell transcript header contains user and machine names. Export
   # only the command output body, never that automatically generated header.
   $text=[regex]::Replace($text,'(?s)\A\s*\*{10,}\r?\n.*?\r?\n\*{10,}\r?\n','')
   $text=[regex]::Replace($text,'(?s)\r?\n\*{10,}\r?\nWindows PowerShell transcript end.*\z','')
  }
  if($truncated){$first=$text.IndexOf("`n");if($first -ge 0){$text=$text.Substring($first+1)}}
  if($relative.EndsWith('.ndjson') -and -not $text.EndsWith("`n")){$last=$text.LastIndexOf("`n");if($last -ge 0){$text=$text.Substring(0,$last+1)}}
  $destination=Join-Path $out $relative;[void](New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force)
  [IO.File]::WriteAllText($destination,(Scrub $text),$utf8)
  $index.files+=@{path=$relative;source_bytes=$length;tail_only=$truncated;sha256=(Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()}
 }catch{$index.issues+=($relative+': '+$_.Exception.Message)}finally{if($stream){$stream.Dispose()}}
}
foreach($file in @('RELEASE.json','TARGET.json','config.json','MANIFEST_SHA256.json','DIAGNOSTICS_SHA256.json')){Copy-Text (Join-Path $root $file) ('build/'+$file)}
foreach($file in @('install-check.json','launch-error.json','runtime-preflight.json')){Copy-Text (Join-Path $reports $file) $file}
$current=Join-Path $root 'CURRENT_SESSION.json';Copy-Text $current 'session.json';$session=$null;$result=$null
try{
 if(Test-Path -LiteralPath $current){
  $session=Get-Content -LiteralPath $current -Raw -Encoding UTF8|ConvertFrom-Json
  $prefix=[IO.Path]::GetFullPath((Join-Path $root 'runs'))+[IO.Path]::DirectorySeparatorChar
  if(-not [IO.Path]::GetFullPath($session.session).StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Recorded session is outside this package.'}
  Copy-Text (Join-Path $session.session 'readiness.json') 'readiness.json'
  $descriptorPath=Join-Path $session.session 'game-control.json';Copy-Text $descriptorPath 'game-control.json'
  if($session.PSObject.Properties['core_result'] -and $session.core_result){$result=[string]$session.core_result}
  elseif(Test-Path -LiteralPath $descriptorPath){$result=[string](Get-Content -LiteralPath $descriptorPath -Raw -Encoding UTF8|ConvertFrom-Json).result_directory}
 }
 if(-not $result){$latest=Get-ChildItem -LiteralPath (Join-Path $root 'RESULTS') -Directory -ErrorAction SilentlyContinue|Where-Object {$_.Name -like 'NOVA_RESULT_*'}|Sort-Object Name -Descending|Select-Object -First 1;if($latest){$result=$latest.FullName}}
 if($result){
  $prefix=[IO.Path]::GetFullPath((Join-Path $root 'RESULTS'))+[IO.Path]::DirectorySeparatorChar
  if(-not [IO.Path]::GetFullPath($result).StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Result path is outside this package.'}
  $index.result_id=Split-Path -Leaf $result
  foreach($file in (Get-ChildItem -LiteralPath $result -File|Where-Object {$_.Name -like 'nova_*.log' -or $_.Name -like 'telemetry*.ndjson' -or $_.Name -in @('RESULT.json','system.json','telemetry-summary.json','telemetry-console.log','telemetry-error.log')})){Copy-Text $file.FullName ('run/'+$file.Name)}
  foreach($file in @('receipt.json','restore.json')){Copy-Text (Join-Path $result ('image-preset/'+$file)) ('run/image-preset/'+$file)}
 }
}catch{$index.issues+=$_.Exception.Message}
if($LaunchLog -and [IO.Path]::GetFullPath($LaunchLog).StartsWith(($reports+[IO.Path]::DirectorySeparatorChar),[StringComparison]::OrdinalIgnoreCase)){Copy-Text $LaunchLog 'launch.log'}
$summary="NOVA CS:GO VR diagnostic report`r`nBuild and game hashes: build/`r`nFPS, XR, controller and tracking timeline: run/telemetry*.ndjson`r`nSummary and hardware: run/telemetry-summary.json and run/system.json`r`nMenu and native input: run/nova_ui.log and run/nova_input.log`r`nHands, shots, effects and stage timings: other run/nova_*.log files`r`nNo automatic upload. Files may be tails; see report-index.json.`r`n"
[IO.File]::WriteAllText((Join-Path $out 'READ_REPORT.txt'),$summary,$utf8)
[IO.File]::WriteAllText((Join-Path $out 'report-index.json'),($index|ConvertTo-Json -Depth 6),$utf8)
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=Join-Path $reports ($name+'.zip');[IO.Compression.ZipFile]::CreateFromDirectory($out,$zip,[IO.Compression.CompressionLevel]::Optimal,$true)
[IO.File]::WriteAllText((Join-Path $reports 'LAST_REPORT.txt'),$zip,$utf8)
$resolved=[IO.Path]::GetFullPath($temporary)
if($resolved.StartsWith(($reports+[IO.Path]::DirectorySeparatorChar+'.collect_'),[StringComparison]::OrdinalIgnoreCase) -and (Split-Path -Leaf $resolved) -match '^\.collect_[0-9a-f]{32}$'){Remove-Item -LiteralPath $resolved -Recurse -Force}
Write-Host ('[REPORT] '+$zip)
[pscustomobject]@{schema='nova.report-result.v1';path=$zip;bytes=(Get-Item -LiteralPath $zip).Length;issues=$index.issues}
