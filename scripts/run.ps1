# CSGOVR NOVA HANDCORE H2 MUZZLE | bounded native P2000 barrel/command component, exact R4 XR host.
# The launcher does not overwrite game files or driver settings. The game may save its own settings.
# Windows PowerShell 5.1. Offline -insecure only. This script is intentionally fail-closed.
param([switch]$BuildOnly,[switch]$HandsOff,[switch]$MuzzleOff,[int]$AutoStopSeconds=0,[switch]$SimBackend,[switch]$Measured,[switch]$UniversalHands,[ValidateSet(1024,1280,1440,2048,2560,3072,3584)][int]$EyeResolution=1024)
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Bin = Join-Path $Root 'bin'
$Result = $null
$ResultFile = $null
$GamePID = 0
$GameProcess = $null
$TrackedGameProcess = $null
$HostProcess = $null
$TelemetryProcess=$null
$MatchEnabled=$false
$HostName=if($SimBackend){'nova_xr64_sim.exe'}else{'nova_xr64_aim.exe'}
$SceneName='nova_scene32.dll'
if($Measured){$HostName=if($SimBackend){'nova_xr64_sim_measured.exe'}else{'nova_xr64_aim_measured.exe'};$SceneName='nova_scene32_measured.dll'}
if($UniversalHands){$HostName=if($SimBackend){'nova_xr64_sim_palm.exe'}else{'nova_xr64_palm.exe'};$SceneName='nova_scene32_universal.dll'}
$RequiredFiles=@($HostName,$SceneName,'nova_inject32.exe','nova_control32.exe','openxr_loader.dll')
$StartedGame = $false
$HostStarted = $false
$Injected = $false
$closeOwnGame=[bool]$UniversalHands
$Status = [ordered]@{
 schema='csgovr.nova.handcore.h2_muzzle.run.v1'; mode='PREPARED_NOT_YET_EXECUTED'; start_utc=[DateTime]::UtcNow.ToString('o')
 target_verified=$false; target_pid=0; xr_host_started=$false; injection_confirmed=$false
 caveat='A successful injector or XR session is not proof of game WORLD stereo or Quest acceptance'
 exit_code=99; cause='UNSTARTED'; launcher_wrote_game_files=$false
}
function Say([string]$s) { Write-Host ('[NOVA] '+$s) }
function LogStatus([string]$cause, [int]$code) { $Status.cause=$cause; $Status.exit_code=$code }
function SHA([string]$p) {$stream=[IO.File]::OpenRead($p);$sha=[Security.Cryptography.SHA256]::Create();try{return [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}}
function Get-PEMachine([string]$p) {
 $f=[IO.File]::OpenRead($p); try {
  $b=New-Object byte[] 4
  if($f.Read($b,0,2) -ne 2 -or $b[0] -ne 77 -or $b[1] -ne 90){return 0}
  $f.Position=0x3c; if($f.Read($b,0,4)-ne 4){return 0}
  $off=[BitConverter]::ToInt32($b,0)
  if($off -lt 64 -or $off -gt $f.Length-6){return 0}
  $f.Position=$off; if($f.Read($b,0,4)-ne 4){return 0}
  if([BitConverter]::ToUInt32($b,0)-ne 0x00004550){return 0}
  if($f.Read($b,0,2)-ne 2){return 0}
  return [BitConverter]::ToUInt16($b,0)
 } finally { $f.Dispose() }
}
function Check-Bin([string]$name, [int]$machine) {
 $p=Join-Path $Bin $name
 if(!(Test-Path -LiteralPath $p -PathType Leaf)){throw "MISSING_REQUIRED_BIN:$name"}
 $m=Get-PEMachine $p
 if($m -ne $machine){throw "WRONG_BINARY_MACHINE:$name`:$(('{0:X4}' -f $m))"}
 return $p
}
function Verify-Package {
 $mf=Join-Path $Root 'MANIFEST_SHA256.json'
 if(!(Test-Path -LiteralPath $mf -PathType Leaf)){throw 'PACKAGE_MANIFEST_MISSING'}
 $manifest=Get-Content -LiteralPath $mf -Raw -Encoding UTF8 | ConvertFrom-Json
 foreach($name in $RequiredFiles){
  $key='bin/'+$name
  $entry=$manifest.files.PSObject.Properties[$key]
  if(!$entry){throw "PACKAGE_MANIFEST_ENTRY_MISSING:$name"}
  if((SHA (Join-Path $Bin $name)) -ne [string]$entry.Value){throw "PACKAGE_BINARY_HASH_MISMATCH:$name"}
 }
}
function Select-OwnProcess([string]$exe,[DateTime]$after,[int]$directPID) {
 # The actual game can be launched through Steam's hand-off. Use exact process executable
 # and creation time, and require -insecure on the actual process command line.
 try {
  $items=@(Get-CimInstance -ClassName Win32_Process -Filter "Name = 'csgo.exe'" -ErrorAction Stop)
  foreach($x in $items){
   if(!$x.ExecutablePath -or $x.ExecutablePath -ine $exe){continue}
   if(!$x.CommandLine -or $x.CommandLine -notmatch '(^|\s)-insecure(\s|$)'){continue}
   if(!$x.CreationDate){continue}
   $ctime=[datetime]$x.CreationDate
   if($ctime.ToUniversalTime() -lt $after.AddSeconds(-2)){continue}
   return [int]$x.ProcessId
  }
 }catch {Say ('CIM_READ_FAILED: '+ $_.Exception.Message)}
 # Safe fallback only for exact starter PID that inherited the launch command line.
 if($directPID -gt 0){
  try{
   $pr=Get-Process -Id $directPID -ErrorAction Stop
   if(!$pr.HasExited -and $pr.Path -ieq $exe -and $pr.StartTime.ToUniversalTime() -ge $after.AddSeconds(-2)){
    return $directPID
   }
  }catch{}
 }
 return 0
}
$DiagnosticsRoot=$Root
$Code=99
$Cause='UNKNOWN'
try {
 $stamp=[DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_fff')
 $Result=Join-Path (Join-Path $Root 'RESULTS') ("NOVA_RESULT_"+$stamp)
 [void](New-Item -ItemType Directory -Force -Path $Result)
 $ResultFile=Join-Path $Result 'RESULT.json'
 Verify-Package
 & (Join-Path $DiagnosticsRoot 'scripts/Verify-LabFiles.ps1')
 [void](Check-Bin $HostName 0x8664)
 [void](Check-Bin $SceneName 0x014c)
 foreach($name in @('nova_inject32.exe','nova_control32.exe')) {
  [void](Check-Bin $name 0x014c)
 }
 [void](Check-Bin 'openxr_loader.dll' 0x8664)
 if($BuildOnly){$Code=0;$Cause='PACKAGE_CHECK_ONLY_NO_GAME';$Status.mode='PACKAGE_CHECK_ONLY';Say 'All binaries have correct Windows architecture. This is not a game execution.';throw 'NOVA_PACKAGE_CHECK_FINISHED'}
 $Config=Get-Content -LiteralPath (Join-Path $Root 'config.json') -Raw -Encoding UTF8 | ConvertFrom-Json
 $MatchEnabled=$UniversalHands -and $Config.PSObject.Properties['session_mode'] -and $Config.session_mode -eq 'MATCH_5V5'
 $Status.play_mode=if($MatchEnabled){'MATCH_5V5_HUMAN_PLUS_NINE_BOTS'}else{'LOCAL_PRACTICE'}
 $GameRoot=[string]$Config.game_directory
 if(-not [IO.Path]::IsPathRooted($GameRoot)){$GameRoot=[IO.Path]::GetFullPath((Join-Path $Root $GameRoot))}
 if(!(Test-Path -LiteralPath $GameRoot -PathType Container)){
  $candidates=@('C:\Program Files (x86)\Steam\steamapps\common\csgo legacy','D:\SteamLibrary\steamapps\common\csgo legacy')
  foreach($c in $candidates){if(Test-Path -LiteralPath $c -PathType Container){$GameRoot=$c;break}}
 }
 $Exe=Join-Path $GameRoot 'csgo.exe';$Client=Join-Path $GameRoot 'csgo\bin\client.dll';$Engine=Join-Path $GameRoot 'bin\engine.dll'
 $Server=Join-Path $GameRoot 'csgo\bin\server.dll';$InputSystem=Join-Path $GameRoot 'bin\inputsystem.dll'
 foreach($file in @($Exe,$Client,$Engine,$Server,$InputSystem)) {if(!(Test-Path -LiteralPath $file -PathType Leaf)){throw "GAME_FILE_MISSING: $file"}}
 foreach($file in @($Exe,$Client,$Engine,$Server,$InputSystem)){if((Get-PEMachine $file) -ne 0x014c){throw "WRONG_GAME_ARCHITECTURE:$file"}}
 $ExeHash=SHA $Exe;$ClientHash=SHA $Client;$EngineHash=SHA $Engine;$ServerHash=SHA $Server;$InputHash=SHA $InputSystem
 if($ExeHash -ne 'de96c3b37e3aaedcf4d6a24c5f6c30acd60f3593e8d452dd9857d682ea3fb114'){throw ('GAME_EXE_HASH_MISMATCH '+$ExeHash)}
 $ExpectedClient='183d42fac8a4ecc24dc02edf5f09122f37fe9e7533ca093a867718b25cfdeb03'
 $ExpectedEngine='bcca98731eab8b72d1ef34fa5fe420f26ac98e8dd1d91698171d0ecd9935faa6'
 if($ClientHash -ne $ExpectedClient -or $EngineHash -ne $ExpectedEngine){throw "TARGET_HASH_MISMATCH client=$ClientHash engine=$EngineHash"}
 $ExpectedServer='699757bb5c8ddb71193f68daba22f59f697491ddbd768bc8656ac2ce435c4db8'
 $ExpectedInput='43bbf7e726ffd511cd423538234bf3a30ec20eceec31987b0f867c0a6f17ef78'
 if($ServerHash -ne $ExpectedServer -or $InputHash -ne $ExpectedInput){throw "GAMEPLAY_TARGET_HASH_MISMATCH server=$ServerHash inputsystem=$InputHash"}
 # Never add hooks to an existing game. Each test runs one newly created offline process.
 $preexisting=@(Get-Process -Name csgo -ErrorAction SilentlyContinue)
 if($preexisting.Count -gt 0){throw 'EXISTING_CSGO_RUNNING__CLOSE_IT_FIRST'}
 $Status.mode='OFFLINE_GAME_TEST';$Status.target_verified=$true
 $Status.build='NOVA_HANDCORE_H2_MUZZLE_R4_HOST';$Status.ipc_version=3
 $Status.native_game_component='NOVA_NATIVE_SCENE_PLUS_H2_COMMAND_BOUND_MUZZLE';$Status.host_component=if($SimBackend){'LAB_GRIP_POINT_AIM_FRAME_EXPLICIT_SIM_TAPE'}else{'LAB_GRIP_POINT_AIM_FRAME_SYSTEM_LOADER'}
 $Status.handcore_mode=if($HandsOff){'HANDS_OFF_BASELINE'}else{'P2000_GRIP_POINT_AIM_FRAME_WITH_LIVE_CALIBRATION'}
 $Status.muzzle_mode=if($HandsOff -or $MuzzleOff){'STOCK_SHOOTING_ORIENTATION_CONTROL'}else{'LOCAL_NATIVE_MUZZLE_EXPERIMENT'}
 if($UniversalHands){
  $Status.build=[string]$Config.build;$Status.fps_features_default=2623
  $Status.native_game_component='NOVA_NATIVE_SCENE_NAMED_PALM_ROOT'
  $Status.host_component=if($SimBackend){'RAW_GRIP_SIM_TAPE'}else{'RAW_GRIP_SYSTEM_LOADER'}
  if(-not $HandsOff){$Status.handcore_mode='LIVE_NAMED_PALM_RAW_GRIP'}
  if(-not ($HandsOff -or $MuzzleOff)){$Status.muzzle_mode='NAMED_FLASH_NATIVE_GUN_CLASS_EXPERIMENT'}
  $Status.controller_input='TOUCH_BUTTONS_STICKS_NATIVE_SHOT_HAPTICS_V2';$Status.monitor_view='NATIVE_LEFT_EYE_MIRROR'
  $Status.schema='nova.universal-palm.ui.run.v1'
 }
 $Status.optics_policy='R3 native centered lens unchanged; exact-sample XR imageRect with pixel-edge FOV; no extra XR-frame idle after copy completion'
 $Status.proof_limit='Source/matrix/packet logs do not certify physical binocular pixels'
 $Status.binaries=[ordered]@{}
 foreach($name in $RequiredFiles){$Status.binaries[$name]=SHA (Join-Path $Bin $name)}
 $Status.binaries['selected_host']=SHA (Join-Path $Bin $HostName)
 $Status.binaries['selected_scene']=SHA (Join-Path $Bin $SceneName)
 $Status.lab_runtime_verified_after_changes=$false
 $Status.target_exe_sha256=$ExeHash;$Status.target_client_sha256=$ClientHash;$Status.target_engine_sha256=$EngineHash;$Status.target_server_sha256=$ServerHash;$Status.target_inputsystem_sha256=$InputHash
 $Status.target_executable=$Exe;$Status.game_directory=$GameRoot
 $Status.architecture_delta='H2: bore mount; naturally requested attachment mask; measured muzzle in command-keyed private record; exact client/server firearm getters; conservative obstruction trace; R4 XR host and WORLD lens retained'
 $Status.architecture_scope='R4_USER_CAMERA_IMAGE_PERFORMANCE_ACCEPTED;H2_PREPARED_LOCAL_MUZZLE_EXPERIMENT;NOT_RUNTIME_GAMEPLAY_VERIFIED'
 if($UniversalHands){$Status.architecture_scope='NAMED_PALM_NATIVE_SHOT_FX_AND_RADIAL_UI;XRSIM_UI_VERIFIED;META_UI_VISUAL_CHECK_NOT_SEPARATELY_CERTIFIED'}
 $Status.before_mutation='Game files checked, never overwritten; this package performs process-local vtable hooks only'
 $ResultFile=Join-Path $Result 'RESULT.json'
 $Status | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $ResultFile -Encoding UTF8
 Say "TARGET EXACT: $GameRoot"
 Say "LOG FOLDER: $Result"
 Say 'Only -insecure offline local games are admitted. Leave online/protected matchmaking closed.'
 $env:NOVA_LOGDIR=$Result
 $env:NOVA_HANDCORE_OFF=if($HandsOff){'1'}else{'0'}
 $env:NOVA_MUZZLE_OFF=if($HandsOff -or $MuzzleOff){'1'}else{'0'}
 # A headset session must keep rendering when the desktop game loses focus.
 # This local hand practice starts both teams with the admitted P2000 model.
 $windowSize=if($UniversalHands){'-w '+$EyeResolution+' -h '+$EyeResolution}else{'-w 1280 -h 1024'}
 if($UniversalHands){$Status.image_preset=& (Join-Path $DiagnosticsRoot 'scripts/Set-ImagePreset.ps1') -BackupDirectory (Join-Path $Result 'image-preset');$Status.launcher_wrote_user_video_config=[bool]$Status.image_preset.changed}
 $arguments='-insecure -novid -windowed '+$windowSize+' +sv_lan 1 +mat_queue_mode 0 +engine_no_focus_sleep 0 +fps_max 144 +map de_dust2 +bot_kick +mp_freezetime 0 +jointeam 3 +slot2'
 if($Config.PSObject.Properties['test_map']){
  if([string]$Config.test_map -notin @('de_dust2','de_nuke')){throw 'Unsupported private fixture map.'}
  $arguments=$arguments.Replace('+map de_dust2',('+map '+[string]$Config.test_map))
 }
 if(-not $UniversalHands){$arguments+=' +mp_ct_default_secondary weapon_hkp2000 +mp_t_default_secondary weapon_hkp2000'}
 $LaunchTime=[DateTime]::UtcNow
 $GameProcess=Start-Process -FilePath $Exe -WorkingDirectory $GameRoot -ArgumentList $arguments -PassThru
 $StartedGame=$true
 $Status.launch_command=$arguments
 $DirectPid=[int]$GameProcess.Id
 for($attempt=0;$attempt -lt 150 -and $GamePID -eq 0;$attempt++) {
  $GamePID=Select-OwnProcess $Exe $LaunchTime $DirectPid
  if($GamePID -eq 0){Start-Sleep -Milliseconds 200}
 }
 if($GamePID -eq 0){throw 'OFFLINE_GAME_NOT_FOUND_EXACT_PROCESS_NO_INJECTION'}
 $TrackedGameProcess=Get-Process -Id $GamePID -ErrorAction Stop
 $null=$TrackedGameProcess.Handle
 $Status.target_pid=$GamePID
 $env:NOVA_PID=[string]$GamePID
 $env:NOVA_GAME=$Exe
 $env:NOVA_DLL=Join-Path $Bin $SceneName
 $Status.selected_scene=$SceneName
 $Status.selected_host=$HostName
 Say "OWN OFFLINE GAME PID: $GamePID"
 if($UniversalHands -and $env:NOVA_LAB_SESSION){
  [ordered]@{schema='nova.game-control.descriptor.v1';pid=$GamePID;map=('Local\NOVA_GAME_CONTROL_'+$GamePID);executable=$Exe;started_utc=$TrackedGameProcess.StartTime.ToUniversalTime().ToString('o');result_directory=$Result} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $env:NOVA_LAB_SESSION 'game-control.json') -Encoding UTF8
 }
 $HostProcess=Start-Process -WindowStyle Hidden -FilePath (Join-Path $Bin $HostName) -WorkingDirectory $Bin -PassThru
 $HostStarted=$true;$Status.xr_host_started=$true
 $Status.xr_host_pid=$HostProcess.Id
 # Host creates the mapping immediately, BEFORE waiting on Oculus Link/OpenXR.
 $MapReady=$false
 for($attempt=0;$attempt -lt 100 -and !$MapReady;$attempt++){
  if($HostProcess.HasExited){throw ('XR_HOST_EXITED_BEFORE_MAPPING code='+$HostProcess.ExitCode)}
  try{
   $MapReady=Test-Path -LiteralPath (Join-Path $Result 'nova_xr64.log') -PathType Leaf
  }catch{}
  if(!$MapReady){Start-Sleep -Milliseconds 100}
 }
 if(!$MapReady){throw 'XR_HOST_LOG_NOT_CREATED'}
 $inject=Start-Process -WindowStyle Hidden -FilePath (Join-Path $Bin 'nova_inject32.exe') -WorkingDirectory $Bin -PassThru -Wait
 $Injected=$inject.ExitCode -eq 0
 $Status.injection_confirmed=$Injected
 $Status.injector_exit_code=$inject.ExitCode
 if(!$Injected){throw ('INJECTOR_FAILED_CODE_'+$inject.ExitCode)}
 if($UniversalHands -and (Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Monitor-Session.ps1'))){
  $telemetryArgs=@('-NoProfile','-ExecutionPolicy','Bypass','-File',('"'+(Join-Path $PSScriptRoot 'Monitor-Session.ps1')+'"'),'-GamePid',[string]$GamePID,'-XrPid',[string]$HostProcess.Id,'-ResultDirectory',('"'+$Result+'"'))
  $TelemetryProcess=Start-Process -WindowStyle Hidden -FilePath (Join-Path $env:SystemRoot 'System32/WindowsPowerShell/v1.0/powershell.exe') -ArgumentList $telemetryArgs -PassThru -RedirectStandardOutput (Join-Path $Result 'telemetry-console.log') -RedirectStandardError (Join-Path $Result 'telemetry-error.log')
 }
 if($SimBackend){& (Join-Path $DiagnosticsRoot 'scripts/Initialize-Sim.ps1') -HostPID $HostProcess.Id}
 Say 'Fresh native scene component admitted to the *exact offline game process*.'
 Say 'XR compositor uses your selected system OpenXR runtime. Put on the headset and activate its PCVR session.'
 if($UniversalHands){
  Say 'Universal hand: wrist and finger-base bones define the palm for the current model. OpenXR grip drives its position and rotation.'
  Say ('Eye textures: '+$EyeResolution+' x '+$EyeResolution+'. F6 pauses hands. Mouse LEFT CLICK fires.')
  Say 'Touch: left stick moves; right stick X turns and down holds quiet walk; A jumps; right stick click crouches; X reloads; B cycles weapons; press Y to drop one weapon (holding does not repeat); left trigger uses E, right grip opens/closes buy, left grip holds quiet walk (also right stick down); AUG/AWP optic image is always on; left stick click shows scores; left menu short press recenters, hold 0.7 seconds opens/closes GameUI. Mouse camera input is suppressed throughout VR. Native server shots drive right-hand haptics.'
 }else{
  Say 'H2 local practice: P2000 is requested for both teams. Hand pose uses grip position and aim orientation; F6 pauses hands and native muzzle.'
  Say 'Mouse LEFT CLICK is still the trigger. Aim and native origin use measured barrel when admitted. Touch buttons/sticks/menu are NOT mapped.'
  Say 'Keep the pistol clear of walls. Missing muzzle or local-server handshake suppresses new primary attack while H2 is admitted.'
 }
 if($MuzzleOff){Say 'MUZZLE OFF CONTROL: bore pose only; shooting remains STOCK.'}
 Say 'Play in the game. CLOSE CS:GO to end and collect the result.'
 if($UniversalHands){Say 'Or type Q + Enter here to close this game and NOVA together.'}else{Say 'Or type Q + Enter here to finish NOVA, leaving the legacy game running.'}
 $Code=0;$Cause='USER_RUN_IN_PROGRESS'
 $qualityApplied=$false;$nextQualityProbe=[DateTime]::UtcNow;$nextJoinProbe=[DateTime]::UtcNow
 $nextReadinessProbe=[DateTime]::UtcNow;$lastReadiness=''
 while($true){
  if($AutoStopSeconds -gt 0 -and ([DateTime]::UtcNow-$LaunchTime).TotalSeconds -ge $AutoStopSeconds){$Cause='LAB_BOUNDED_STOP';$closeOwnGame=$true;break}
  if($HostProcess.HasExited){$HostEC=$HostProcess.ExitCode;$Code=if($HostEC -eq 0){0}else{20};$Cause='XR_HOST_EXITED_CODE_'+$HostEC;break}
  try{$alive=Get-Process -Id $GamePID -ErrorAction SilentlyContinue}catch{$alive=$null}
  if(!$alive){$Cause='GAME_PROCESS_EXITED';break}
  # The game's archived config can overwrite +commands during map loading.
  # Prepare the selected local mode after the player joins, including while Meta waits
  # for the physical headset to enter its active Link session.
  if($UniversalHands -and -not $qualityApplied -and [DateTime]::UtcNow -ge $nextQualityProbe){
   $nextQualityProbe=[DateTime]::UtcNow.AddSeconds(1)
   try{
    $native=& (Join-Path $DiagnosticsRoot 'scripts/Control-Game.ps1') -GamePid $GamePID | ConvertFrom-Json
    if($MatchEnabled -and $native.team -notin @(2,3) -and $native.life -ne 255 -and [DateTime]::UtcNow -ge $nextJoinProbe){
     $nextJoinProbe=[DateTime]::UtcNow.AddSeconds(5)
     $Status.match_join=& (Join-Path $DiagnosticsRoot 'scripts/Control-Game.ps1') -GamePid $GamePID -Command 'jointeam 3; gameui_hide' | ConvertFrom-Json
    }
    if($native.team -in @(2,3) -and $native.model){
     if($MatchEnabled){
      $preset=@('sv_cheats 1; engine_no_focus_sleep 0; fps_max 144; cl_use_opens_buy_menu 0; mp_autoteambalance 1; mp_limitteams 1; mp_humanteam any; mp_respawn_on_death_ct 0; mp_respawn_on_death_t 0; mp_ignore_round_win_conditions 0; mp_warmup_pausetimer 0; mp_freezetime 6; mp_buy_anywhere 0; mp_buytime 20; mp_startmoney 800; mp_maxrounds 30; mp_halftime 1; mp_roundtime_defuse 1.92','bot_kick; bot_quota_mode normal; bot_join_team any; bot_join_after_player 1; bot_auto_vacate 1; bot_difficulty 2; bot_stop 0; bot_zombie 0; bot_dont_shoot 0; bot_mimic 0; bot_ignore_players 0; bot_quota 9; mp_warmup_end; mp_restartgame 1')
     }else{$preset=@('sv_cheats 1; engine_no_focus_sleep 0; fps_max 144; bot_quota 0; bot_kick; mp_autoteambalance 0; mp_limitteams 0; mp_respawn_on_death_ct 1; mp_respawn_on_death_t 1; mp_warmup_start; mp_warmup_pausetimer 1; slot2')}
     $receipt=@(foreach($command in $preset){& (Join-Path $DiagnosticsRoot 'scripts/Control-Game.ps1') -GamePid $GamePID -Command $command | ConvertFrom-Json})
     $imagePreset=if($env:NOVA_IMAGE_AA -eq '0'){'NATIVE_RAW_AF16_USER_ACCEPTED_META'}else{'XR_FXAA_SHARPEN_AF16'}
     $Status.image_quality=[ordered]@{preset=$imagePreset;eye_resolution=$EyeResolution;native_commands=$preset;receipt=$receipt;native_value_readback='NOT_IMPLIED_BY_QUEUE_RECEIPT'}
     $qualityApplied=$true
     if($MatchEnabled){Say ('Local 5v5 match prepared: nine bots, normal rounds and buy zones. Image preset: '+$imagePreset)}
     else{Say ('Local practice prepared without bots; unfocused rendering without sleep. Image preset: '+$imagePreset)}
    }
   }catch{$Status.image_quality_probe=$_.Exception.Message}
  }
  if($UniversalHands -and [DateTime]::UtcNow -ge $nextReadinessProbe){
   $nextReadinessProbe=[DateTime]::UtcNow.AddSeconds(2)
   try{
    $readiness=& (Join-Path $DiagnosticsRoot 'scripts/Get-RunState.ps1') -GamePid $GamePID | ConvertFrom-Json
    $Status.readiness=$readiness
    $Status.cause='USER_RUN_IN_PROGRESS';$Status.exit_code=$null
    $Status|ConvertTo-Json -Depth 9|Set-Content -LiteralPath $ResultFile -Encoding UTF8
    if($env:NOVA_LAB_SESSION){$readiness|ConvertTo-Json -Depth 7|Set-Content -LiteralPath (Join-Path $env:NOVA_LAB_SESSION 'readiness.json') -Encoding UTF8}
    if($readiness.stage -ne $lastReadiness){$lastReadiness=$readiness.stage;Say ($readiness.stage+': '+$readiness.message)}
   }catch{$Status.readiness_error=$_.Exception.Message}
  }
  if(-not [Console]::IsInputRedirected -and [Console]::KeyAvailable){
   $key=[Console]::ReadKey($true)
   if($key.KeyChar -eq 'q' -or $key.KeyChar -eq 'Q'){$Cause='USER_COOPERATIVE_STOP';$closeOwnGame=$true;break}
  }
  Start-Sleep -Milliseconds 300
 }
}catch{
 if($BuildOnly -and $_.Exception.Message -eq 'NOVA_PACKAGE_CHECK_FINISHED'){$Code=0;$Cause='PACKAGE_CHECK_ONLY_NO_GAME'}else{
  $Code=31;$Cause=$_.Exception.Message
  Say ('FAIL: '+$Cause)
 }
}finally{
 if($Result){
  # Ask the exact owned native game to release input/resources before its quit.
  if($UniversalHands -and $closeOwnGame -and $GamePID -gt 0 -and $TrackedGameProcess -and -not $TrackedGameProcess.HasExited){
   try{[void](& (Join-Path $DiagnosticsRoot 'scripts/Control-Game.ps1') -GamePid $GamePID -StopGame);$Status.cooperative_game_close='QUEUED'}catch{$Status.cooperative_game_close=$_.Exception.Message}
   try{[void]$TrackedGameProcess.WaitForExit(8000)}catch{}
   if(-not $TrackedGameProcess.HasExited){$TrackedGameProcess.Kill();[void]$TrackedGameProcess.WaitForExit(4000);$Status.owned_game_close_fallback='EXACT_STARTED_PROCESS_TERMINATED_AFTER_NATIVE_CLOSE_TIMEOUT'}
  }
  # A cooperative test stop can precede its queued engine quit by one frame.
  if($AutoStopSeconds -gt 0 -and $TrackedGameProcess){try{[void]$TrackedGameProcess.WaitForExit(4000)}catch{}}
  # Cooperative no-hot-unload: restore vtable pointers, host releases its own resources.
  if($GamePID -gt 0 -and $HostStarted -and !$HostProcess.HasExited){
   try{$ctl=Start-Process -WindowStyle Hidden -FilePath (Join-Path $Bin 'nova_control32.exe') -WorkingDirectory $Bin -PassThru -Wait
    $Status.control_exit_code=$ctl.ExitCode
   }catch{$Status.control_error=$_.Exception.Message}
   Start-Sleep -Milliseconds 350
   try{$HostProcess.WaitForExit(4000) | Out-Null}catch{}
  }
  $Status.exit_code=$Code;$Status.cause=$Cause;$Status.end_utc=[DateTime]::UtcNow.ToString('o')
  if($HostProcess){try{$Status.xr_host_exit_code=if($HostProcess.HasExited){$HostProcess.ExitCode}else{'STILL_RUNNING'}}catch{$Status.xr_host_exit_code='UNKNOWN'}}
 
  if($TrackedGameProcess){
   try{$Status.game_exit_code=if($TrackedGameProcess.HasExited){$TrackedGameProcess.ExitCode}else{'STILL_RUNNING'}}catch{$Status.game_exit_code='UNKNOWN';$Status.game_exit_code_error=$_.Exception.Message}
   if($Status.game_exit_code -is [int] -and $Status.game_exit_code -ne 0 -and $Code -eq 0){$Code=21;$Cause='GAME_EXIT_NONZERO_'+('{0:X8}' -f $Status.game_exit_code);$Status.exit_code=$Code;$Status.cause=$Cause}
  }
  if($GamePID -gt 0 -and $TrackedGameProcess -and $TrackedGameProcess.HasExited){
   $Status.application_error_check='COMPLETED';$Status.application_errors=@()
   try{
    Start-Sleep -Milliseconds 1000
    $appEvents=@(Get-WinEvent -FilterHashtable @{LogName='Application';Id=1000;StartTime=$LaunchTime.ToLocalTime()} -ErrorAction SilentlyContinue)
    foreach($event in $appEvents){
     $v=$event.Properties
     if($event.ProviderName -eq 'Application Error' -and $v.Count -ge 11 -and [string]$v[0].Value -ieq 'csgo.exe' -and [Convert]::ToInt32([string]$v[8].Value,16) -eq $GamePID -and [string]$v[10].Value -ieq $Exe){
      $Status.application_errors+=@{utc=$event.TimeCreated.ToUniversalTime().ToString('o');module=[string]$v[3].Value;exception=[string]$v[6].Value;offset=[string]$v[7].Value;pid=$GamePID}
     }
    }
    if($Status.application_errors.Count -gt 0 -and $Code -eq 0){$Code=21;$Cause='GAME_APPLICATION_ERROR_RECORDED';$Status.exit_code=$Code;$Status.cause=$Cause}
   }catch{$Status.application_error_check='UNAVAILABLE';$Status.application_error_check_detail=$_.Exception.Message}
  }
  if($TelemetryProcess){
   [IO.File]::WriteAllText((Join-Path $Result 'telemetry.stop'),'stop')
   try{[void]$TelemetryProcess.WaitForExit(4000);$Status.telemetry_exit=if($TelemetryProcess.HasExited){$TelemetryProcess.ExitCode}else{'STILL_FINISHING'}}catch{$Status.telemetry_error=$_.Exception.Message}
  }
  if((Test-Path -LiteralPath (Join-Path $Result 'image-preset/receipt.json')) -and (Test-Path -LiteralPath (Join-Path $PSScriptRoot 'Restore-ImagePreset.ps1')) -and (-not $TrackedGameProcess -or $TrackedGameProcess.HasExited)){
   try{$Status.image_preset_restore=& (Join-Path $PSScriptRoot 'Restore-ImagePreset.ps1') -BackupDirectory (Join-Path $Result 'image-preset')}catch{$Status.image_preset_restore=[ordered]@{status='RESTORE_ERROR';message=$_.Exception.Message}}
  }
  $Status.runtime_acceptance='NOT_INFERRED_FROM_SUCCESSFUL_LAUNCH'
  $gp=Join-Path $Result 'nova_game32.log';$hp=Join-Path $Result 'nova_xr64.log'
  $Status.observed=[ordered]@{r3_game_marker=$false;r4_host_marker=$false;camera_region_records=$false;exact_sample_regions=$false;native_full_fallback=$false;source_eye_packets=$false;latched_pose_packets=$false;submitted_pose_packets=$false}
  if(Test-Path -LiteralPath $gp){$gtext=Get-Content -LiteralPath $gp -Raw -ErrorAction SilentlyContinue;$Status.observed.r3_game_marker=($gtext -match 'R3_BUILD_NATIVE_LENS');$Status.observed.source_eye_packets=($gtext -match 'NOVA_R3_EYE')}
  if(Test-Path -LiteralPath $hp){$htext=Get-Content -LiteralPath $hp -Raw -ErrorAction SilentlyContinue;$Status.observed.r4_host_marker=($htext -match 'R4_HOST_CAMERA_BUILD_R3_NATIVE_PRESERVED');$Status.observed.camera_region_records=($htext -match 'NOVA_R4_CAMERA');$Status.observed.exact_sample_regions=($htext -match 'mode=EXACT_SAMPLE_REGION');$Status.observed.native_full_fallback=($htext -match 'mode=NATIVE_FULL_FALLBACK');$Status.observed.latched_pose_packets=($htext -match 'R3_PAIR_METADATA_LATCHED');$Status.observed.submitted_pose_packets=($htext -match 'R3_SUBMIT_IMAGE_POSE')}
  $handLog=Join-Path $Result 'nova_handcore_h1.log'
  $Status.handcore=[ordered]@{log_present=$false;build_marker=$false;detailed_gate_records=$false;last_gate_record=$null;root_applied_observed=$false;passive_palette_sample=$false;retired_observed=$false;last_counts_record=$null;visual_or_gameplay_acceptance='NOT_INFERRED'}
  if(Test-Path -LiteralPath $handLog){
   $text=Get-Content -LiteralPath $handLog -Raw -ErrorAction SilentlyContinue
   $Status.handcore.log_present=$true
   $Status.handcore.build_marker=($text -match 'NOVA_HANDCORE_H2_BORE_BUILD')
   $gateRows=[regex]::Matches([string]$text,'NOVA_H1F1_GATE[^\r\n]*')
   $Status.handcore.detailed_gate_records=($gateRows.Count -gt 0)
   if($gateRows.Count -gt 0){$Status.handcore.last_gate_record=$gateRows[$gateRows.Count-1].Value}
   $Status.handcore.root_applied_observed=($text -match 'NOVA_H1_ROOT')
   $Status.handcore.passive_palette_sample=($text -match 'NOVA_H1_NATURAL_PALM')
   $Status.handcore.retired_observed=($text -match 'retired=1')
   $matches=[regex]::Matches([string]$text,'NOVA_H1_COUNTS[^\r\n]*')
   if($matches.Count -gt 0){$Status.handcore.last_counts_record=$matches[$matches.Count-1].Value}
   if(!$Status.handcore.root_applied_observed){
    Say 'HANDCORE: no native root application recorded. Inspect tracking and the hand gate before playing.'
    if($Status.handcore.last_gate_record){Say $Status.handcore.last_gate_record}
   }
  }
  $muzzleLog=Join-Path $Result 'nova_muzzle_h2.log'
  $Status.muzzle=[ordered]@{log_present=$false;native_boundaries_ready=$false;natural_paired_muzzle=$false;command_records=$false;server_command_bound=$false;client_shot_records=0;server_shot_records=0;server_initial_trace_records=0;admission_failed=$false;last_guard_record=$null;hit_damage_or_visual_acceptance='NOT_CERTIFIED_BY_THIS_LOG'}
  if(Test-Path -LiteralPath $muzzleLog){
   $mt=[string](Get-Content -LiteralPath $muzzleLog -Raw -ErrorAction SilentlyContinue)
   $Status.muzzle.log_present=$true
   $Status.muzzle.native_boundaries_ready=($mt -match 'NOVA_H2_NATIVE_MUZZLE_BOUNDARIES_READY')
   $Status.muzzle.natural_paired_muzzle=($mt -match 'NOVA_H2_NATURAL_MUZZLE')
   $Status.muzzle.command_records=($mt -match 'NOVA_H2_COMMAND number=')
   $Status.muzzle.server_command_bound=($mt -match 'NOVA_H2_LOCAL_SERVER_COMMAND_BOUND')
   $Status.muzzle.client_shot_records=([regex]::Matches($mt,'NOVA_H2_NATIVE_SHOT side=CLIENT')).Count
   $Status.muzzle.server_shot_records=([regex]::Matches($mt,'NOVA_H2_NATIVE_SHOT side=SERVER')).Count
   $Status.muzzle.server_initial_trace_records=([regex]::Matches($mt,'NOVA_H2_SERVER_INITIAL_BULLET_TRACE')).Count
   $Status.muzzle.admission_failed=($mt -match 'ADMISSION_FAILED|VERIFY_READ_FAILED|CRC_MISMATCH')
   $gr=[regex]::Matches($mt,'NOVA_H2_COUNTS_GUARDS[^\r\n]*');if($gr.Count -gt 0){$Status.muzzle.last_guard_record=$gr[$gr.Count-1].Value}
   Say ('MUZZLE receipts: client='+$Status.muzzle.client_shot_records+' server='+$Status.muzzle.server_shot_records+' initial trace='+$Status.muzzle.server_initial_trace_records+' (not a damage/visual certificate)')
  }
  try{$Status | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $ResultFile -Encoding UTF8}catch{}
  try{
   if($env:NOVA_PORTABLE_MODE -ne '1'){$ResultZip=$Result+'.zip';Compress-Archive -LiteralPath $Result -DestinationPath $ResultZip -CompressionLevel Optimal -Force;Say "RESULT ZIP: $ResultZip"}
  }catch{
   $Code=41;$Cause='RESULT_ZIP_FAILED';$Status.exit_code=$Code;$Status.cause=$Cause
   $Status.result_packaging_error=$_.Exception.Message
   Say ('RESULT ZIP ERROR: '+$Status.result_packaging_error)
  try{$Status | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $ResultFile -Encoding UTF8}catch{}
  }
 }
 # Universal sessions close only their exact owned game process; legacy sessions keep their existing lifecycle.
 Say ('VERDICT: '+$Cause)
}
exit $Code
