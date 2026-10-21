[CmdletBinding()]
param(
 [ValidateSet('Hand','Graphics','Graphics3K')][string]$Profile='Hand',
 [ValidateSet('Sim','Headset')][string]$Runtime='Sim',
 [ValidateRange(0,86400)][int]$MaxSeconds=0,
 [switch]$CheckOnly,
 [switch]$HeadsetTrace,
 [switch]$NoTrace,
 [switch]$NoAutoCapture,
 [switch]$Measured,
 [switch]$UniversalHands,
 [ValidateSet(1024,1280,1440,2048,2560,3072,3584)][int]$EyeResolution=1024,
 [switch]$PoseMarkers,
 [switch]$Match3KBackbuffer,
 [switch]$AdaptiveNative=$true,
 [switch]$HandsOff,
 [switch]$MuzzleOff
)
$ErrorActionPreference='Stop'
if($Profile -eq 'Graphics3K'){$Profile='Graphics'}
if($Measured -and $Profile -ne 'Hand'){throw 'Measured is available for the Hand profile.'}
if($UniversalHands -and $Profile -ne 'Hand'){throw 'UniversalHands is available for the Hand profile.'}
$lab=Split-Path -Parent $PSScriptRoot
if(-not $CheckOnly){
 & (Join-Path $PSScriptRoot 'Assert-GameIsolation.ps1')
}
$session=Join-Path (Join-Path $lab 'runs') ([DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_fff')+'_'+$Profile+'_'+$Runtime)
[void](New-Item -ItemType Directory -Path $session -Force)
$sim=Join-Path $lab 'diagnostics\xr-sim'
$tape=Join-Path $lab 'diagnostics\xr-tape'
$profileRoot=if($Profile -eq 'Hand'){$lab}else{Join-Path $lab 'profiles\graphics3k'}
$names=@('XR_RUNTIME_JSON','XRSIM_DIR','XRSIM_HEADSET_CONFIG','XR_API_LAYER_PATH','XR_ENABLE_API_LAYERS','XRTAPE_DIR','XRTAPE_MAX_FRAMES','NOVA_LAB_SESSION','NOVA_LAB_ROOT','NOVA_LOGDIR','NOVA_PID','NOVA_GAME','NOVA_DLL','NOVA_HANDCORE_OFF','NOVA_MUZZLE_OFF','NOVA_R7_MODE','NOVA_EYE_RESOLUTION','NOVA_LAB_MARKERS','NOVA_IMAGE_AA')
$saved=@{}
foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
$traceEnabled=(-not $CheckOnly) -and (-not $NoTrace) -and ($Runtime -eq 'Sim' -or $HeadsetTrace)
$status=[ordered]@{schema='nova.xr-lab.session.v2';profile=$Profile;runtime=$Runtime;trace_enabled=$traceEnabled;session=$session;sim_directory=$null;started_utc=[DateTime]::UtcNow.ToString('o');exit_code=$null;stage='starting';game_launch_requested=(-not $CheckOnly)}
$status.hand_mode=if($UniversalHands){'NAMED_PALM_RAW_GRIP'}else{'P2000_AIM_FRAME'}
$status.eye_resolution=$EyeResolution
$status.pose_markers=[bool]$PoseMarkers
function Save-LabStatus {
 $json=$status | ConvertTo-Json -Depth 6
 [IO.File]::WriteAllText((Join-Path $session 'session.json'),$json,[Text.UTF8Encoding]::new($false))
 if(-not $CheckOnly){[IO.File]::WriteAllText((Join-Path $lab 'CURRENT_SESSION.json'),$json,[Text.UTF8Encoding]::new($false))}
}
try {
 & (Join-Path $PSScriptRoot 'Verify-LabFiles.ps1')
 $principal=[Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
 if($traceEnabled -and $Runtime -eq 'Headset' -and $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)){throw 'Optional HeadsetTrace requires a normal unelevated launch. Omit -HeadsetTrace to use the installed headset runtime.'}
 if($Runtime -eq 'Sim'){
  $env:XR_RUNTIME_JSON=Join-Path $sim 'runtime.json'
  $env:XRSIM_DIR=Join-Path $session 'sim'
  [void](New-Item -ItemType Directory -Path $env:XRSIM_DIR -Force)
  Remove-Item Env:XRSIM_HEADSET_CONFIG -ErrorAction SilentlyContinue
  $status.sim_directory=$env:XRSIM_DIR
  # Commands are sent only AFTER this new runtime reports a running session.
  # Pre-instance command files are deliberately ignored by xr-sim.
 }else{
  # The headset uses the installed OpenXR runtime, without inherited simulator overrides.
  foreach($name in @('XR_RUNTIME_JSON','XRSIM_DIR','XRSIM_HEADSET_CONFIG')){[Environment]::SetEnvironmentVariable($name,$null,'Process')}
 }
 if($traceEnabled){
  $env:XR_API_LAYER_PATH=$tape
  $env:XR_ENABLE_API_LAYERS='XR_APILAYER_XRTAPE_recorder'
  $env:XRTAPE_DIR=Join-Path $session 'tape'
  [void](New-Item -ItemType Directory -Path $env:XRTAPE_DIR -Force)
  $env:XRTAPE_MAX_FRAMES='1000000'
 }else{
  foreach($name in @('XR_API_LAYER_PATH','XR_ENABLE_API_LAYERS','XRTAPE_DIR','XRTAPE_MAX_FRAMES')){[Environment]::SetEnvironmentVariable($name,$null,'Process')}
  if($Runtime -eq 'Sim'){
   # The proven simulator host always dispatches through its application-owned
   # tape DLL. Keep its small startup recording local even without full checks.
   $env:XRTAPE_DIR=Join-Path $session 'tape';[void](New-Item -ItemType Directory -Path $env:XRTAPE_DIR -Force)
   $env:XRTAPE_MAX_FRAMES='240';$status.sim_startup_recording='BOUNDED_240_FRAMES_NO_TRACE_CHECK_REQUESTED'
  }
 }
 $env:NOVA_LAB_SESSION=$session
 $env:NOVA_LAB_ROOT=$lab
 $env:NOVA_EYE_RESOLUTION=[string]$EyeResolution
 if($UniversalHands){
  if($Runtime -eq 'Headset'){$env:NOVA_IMAGE_AA='0';$status.image_quality='NATIVE_RAW_AF16_USER_ACCEPTED_META'}
  else{$env:NOVA_IMAGE_AA='1';$status.image_quality='FXAA_SHARPEN_025_AF16_HIGH_TEXTURES_SIM'}
 }
 $env:NOVA_LAB_MARKERS=if($PoseMarkers){'1'}else{'0'}
 Save-LabStatus
 Write-Host ('[NOVA XR LAB] '+$Profile+' / '+$Runtime+' -> '+$session)
 $arguments=@{AutoStopSeconds=$MaxSeconds;SimBackend=($Runtime -eq 'Sim');BuildOnly=$CheckOnly}
 if($Profile -eq 'Hand'){$arguments.HandsOff=$HandsOff;$arguments.MuzzleOff=$MuzzleOff;$arguments.Measured=$Measured;$arguments.UniversalHands=$UniversalHands;$arguments.EyeResolution=$EyeResolution}
 else{$arguments.MatchNativeWindow=$Match3KBackbuffer;$arguments.AdaptiveNative=$AdaptiveNative}
 if(-not $CheckOnly -and -not (Get-Process -Name steam -ErrorAction SilentlyContinue)){
  $steam=Join-Path ${env:ProgramFiles(x86)} 'Steam/steam.exe'
  try{$savedSteam=[string](Get-ItemProperty 'HKCU:/Software/Valve/Steam' -ErrorAction Stop).SteamPath;if($savedSteam){$steam=Join-Path $savedSteam 'steam.exe'}}catch{}
  if(Test-Path -LiteralPath $steam){[void](Start-Process -FilePath $steam -WindowStyle Hidden -PassThru);Start-Sleep -Seconds 3}
 }
 $status.stage=if($CheckOnly){'checking'}else{'running'};Save-LabStatus
 if(-not $CheckOnly -and $Runtime -eq 'Sim' -and -not $NoAutoCapture){
  $observerArgs=@('-NoProfile','-ExecutionPolicy','Bypass','-File',('"'+(Join-Path $PSScriptRoot 'Observe-Stereo.ps1')+'"'),'-SessionFile',('"'+(Join-Path $session 'session.json')+'"'))
  $observer=Start-Process -WindowStyle Hidden -FilePath (Join-Path $env:SystemRoot 'System32/WindowsPowerShell/v1.0/powershell.exe') -ArgumentList $observerArgs -PassThru -RedirectStandardOutput (Join-Path $session 'observer.log') -RedirectStandardError (Join-Path $session 'observer-error.log')
 }
 & (Join-Path $profileRoot 'scripts\run.ps1') @arguments
 $status.exit_code=$LASTEXITCODE
 $status.core_result=$env:NOVA_LOGDIR
} catch {
 $status.stage='failed';$status.error=$_.Exception.Message;$status.exit_code=1
 Write-Host ('[NOVA XR LAB] FAILED: '+$_.Exception.Message) -ForegroundColor Red
} finally {
 foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
 if($status.stage -ne 'failed'){$status.stage=if($CheckOnly){'checked'}else{'stopped'}}
 $status.ended_utc=[DateTime]::UtcNow.ToString('o')
 if($CheckOnly){$status.trace_status='PACKAGE_CHECK_ONLY_NO_GAME'}elseif(-not $traceEnabled){$status.trace_status='NOT_REQUESTED'}else{
  try{
   $traceCheck=& (Join-Path $PSScriptRoot 'Check-Traces.ps1') -Session $session -Runtime $Runtime
   $status.trace_status=$traceCheck.status
   if($status.exit_code -eq 0 -and $traceCheck.exit_code -ne 0){$status.exit_code=$traceCheck.exit_code}
  }catch{$status.trace_status='CHECK_TOOL_ERROR';$status.trace_error=$_.Exception.Message;if($status.exit_code -eq 0){$status.exit_code=42}}
 }
 Save-LabStatus
 Write-Host ('[NOVA XR LAB] Result: '+$session+' | exit='+$status.exit_code+' | trace='+$status.trace_status)
}
exit $status.exit_code
