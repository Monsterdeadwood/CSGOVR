[CmdletBinding()]
param([ValidateSet('Menu','Meta','Fast','Sim','Check','Report','Stop','Status')][string]$Action='Menu',[ValidateRange(0,86400)][int]$MaxSeconds=0)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$reports=Join-Path $root 'REPORTS'
[void](New-Item -ItemType Directory -Path $reports -Force)
function Invoke-Action([string]$Choice){
 $code=0;$launchLog='';$transcribing=$false
 try{
  switch($Choice){
   'Report' {& (Join-Path $PSScriptRoot 'Collect-Report.ps1')|Out-Host;return 0}
   'Stop' {& (Join-Path $PSScriptRoot 'Control-Game.ps1') -StopGame|Out-Host;return 0}
   'Status' {& (Join-Path $PSScriptRoot 'Get-RunState.ps1')|Out-Host;return 0}
  }
  $launchLog=Join-Path $reports ('launch_'+[DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_fff')+'.log')
  try{Start-Transcript -Path $launchLog -Force|Out-Null;$transcribing=$true}catch{}
  & (Join-Path $PSScriptRoot 'Get-RuntimePreflight.ps1') -OutputFile (Join-Path $reports 'runtime-preflight.json') -Action $Choice
  & (Join-Path $PSScriptRoot 'Test-PortableInstall.ps1')|Out-Host
  $oldError=Join-Path $reports 'launch-error.json';if(Test-Path -LiteralPath $oldError){[IO.File]::Delete($oldError)}
  if($Choice -in @('Meta','Fast','Sim') -and -not (Get-Process csgo -ErrorAction SilentlyContinue)){
   $results=Join-Path $root 'RESULTS'
   if(Test-Path -LiteralPath $results){foreach($old in (Get-ChildItem -LiteralPath $results -Directory -Filter 'NOVA_RESULT_*')){
    $backup=Join-Path $old.FullName 'image-preset';$receipt=Join-Path $backup 'receipt.json';$restore=Join-Path $backup 'restore.json'
    if(-not (Test-Path -LiteralPath $receipt)){continue}
    if((Test-Path -LiteralPath $restore) -and (Get-Content -LiteralPath $restore -Raw -Encoding UTF8|ConvertFrom-Json).status -like 'RESTORED*'){continue}
    & (Join-Path $PSScriptRoot 'Restore-ImagePreset.ps1') -BackupDirectory $backup|Out-Host
   }}
  }
  $savedPortable=$env:NOVA_PORTABLE_MODE
  try{
   $env:NOVA_PORTABLE_MODE='1'
   $arguments=@{Profile='Hand';UniversalHands=$true;NoTrace=$true;NoAutoCapture=$true;EyeResolution=2560;MaxSeconds=$MaxSeconds}
   if($Choice -eq 'Fast'){$arguments.EyeResolution=2048}
   $arguments.Runtime=if($Choice -eq 'Sim'){'Sim'}else{'Headset'}
   if($Choice -eq 'Check'){$arguments.CheckOnly=$true}
   & (Join-Path $PSScriptRoot 'Start-Nova.ps1') @arguments
   $code=$LASTEXITCODE
   if($code -ne 0){throw ('Session ended with code '+$code+'. See the diagnostic report.')}
  }finally{$env:NOVA_PORTABLE_MODE=$savedPortable}
 }catch{
  if(-not $code){$code=1}
  $errorRecord=[ordered]@{schema='nova.portable.launch-error.v1';utc=[DateTime]::UtcNow.ToString('o');action=$Choice;exit_code=$code;error=$_.Exception.Message}
  $errorRecord|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $reports 'launch-error.json') -Encoding UTF8
  Write-Host ('[NOVA] '+$_.Exception.Message) -ForegroundColor Red
 }finally{
  if($transcribing){try{Stop-Transcript|Out-Null}catch{}}
  if($launchLog){try{& (Join-Path $PSScriptRoot 'Collect-Report.ps1') -LaunchLog $launchLog|Out-Host}catch{Write-Host ('[REPORT ERROR] '+$_.Exception.Message) -ForegroundColor Red;if(-not $code){$code=41}}}
 }
 return $code
}
if($Action -ne 'Menu'){exit (Invoke-Action $Action)}
while($true){
 Write-Host ''
 Write-Host 'NOVA - CS:GO Legacy VR'
 Write-Host '1  System OpenXR runtime - 5v5 match, 2560 per eye'
 Write-Host '2  System OpenXR runtime - 5v5 match, 2048 per eye'
 Write-Host 'Use the active OpenXR runtime selected in Meta, SteamVR or Virtual Desktop.'
 Write-Host 'D  Save diagnostic ZIP (also works during play)'
 Write-Host 'T  Current session status'
 Write-Host 'S  Close this package game'
 Write-Host 'C  Check installation without launching'
 Write-Host 'Q  Exit launcher'
 $selected=Read-Host 'Select'
 switch($selected.ToUpperInvariant()){
  '1' {$null=Invoke-Action 'Meta'}
  '2' {$null=Invoke-Action 'Fast'}
  'D' {$null=Invoke-Action 'Report'}
  'T' {$null=Invoke-Action 'Status'}
  'S' {$null=Invoke-Action 'Stop'}
  'C' {$null=Invoke-Action 'Check'}
  'Q' {exit 0}
 }
}
