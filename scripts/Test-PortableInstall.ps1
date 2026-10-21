[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$game=[IO.Path]::GetFullPath((Join-Path $root '..'))
$report=[ordered]@{schema='nova.portable.install-check.v1';utc=[DateTime]::UtcNow.ToString('o');status='CHECKING';game_directory=$game;package_directory=$root;game_files=@{};package_files_checked=0}
try{
 if(-not [Environment]::Is64BitOperatingSystem){throw 'Windows x64 is required.'}
 $manifest=Get-Content -LiteralPath (Join-Path $root 'MANIFEST_SHA256.json') -Raw -Encoding UTF8|ConvertFrom-Json
 foreach($file in $manifest.files.PSObject.Properties){
  $path=Join-Path $root $file.Name
  if(-not (Test-Path -LiteralPath $path -PathType Leaf)){throw ('Extract the whole folder. Missing: '+$file.Name)}
  if((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant() -ne $file.Value){throw ('Package file changed: '+$file.Name)}
  $report.package_files_checked++
 }
 $target=Get-Content -LiteralPath (Join-Path $root 'TARGET.json') -Raw -Encoding UTF8|ConvertFrom-Json
 foreach($file in $target.files.PSObject.Properties){
  $path=Join-Path $game $file.Name
  if(-not (Test-Path -LiteralPath $path -PathType Leaf)){throw 'Put this whole folder next to csgo.exe in CS:GO Legacy. CS2 is not supported.'}
  $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant();$report.game_files[$file.Name]=$hash
  if($hash -ne $file.Value){throw ('Unsupported game build: '+$file.Name+'. Diagnostic report contains the detected hash.')}
 }
 $report.status='FILES_VERIFIED_NO_GAME_LAUNCHED'
}catch{$report.status='CHECK_FAILED';$report.error=$_.Exception.Message}
finally{
 [void](New-Item -ItemType Directory -Path (Join-Path $root 'REPORTS') -Force)
 [IO.File]::WriteAllText((Join-Path $root 'REPORTS/install-check.json'),($report|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
}
if($report.status -eq 'CHECK_FAILED'){throw $report.error}
[pscustomobject]$report
