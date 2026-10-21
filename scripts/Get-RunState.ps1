[CmdletBinding()]
param([int]$GamePid=0,[ValidateRange(50,1000)][int]$ObserveMilliseconds=150,[string]$SessionFile='')
$ErrorActionPreference='Stop'
$lab=Split-Path -Parent $PSScriptRoot
$descriptor=$null
$output=[ordered]@{schema='nova.run-state.v1';utc=[DateTime]::UtcNow.ToString('o');pid=$GamePid;stage='STARTING';message='Starting the owned game and XR host.';physical_visual_acceptance='NOT_INFERRED'}
$owned=New-Object 'System.Collections.Generic.List[System.IDisposable]'
function Open-StatusMap([string]$Name,[int]$Bytes){
 $mapping=[IO.MemoryMappedFiles.MemoryMappedFile]::OpenExisting($Name,[IO.MemoryMappedFiles.MemoryMappedFileRights]::Read);$owned.Add($mapping)
 $view=$mapping.CreateViewAccessor(0,$Bytes,[IO.MemoryMappedFiles.MemoryMappedFileAccess]::Read);$owned.Add($view);return $view
}
function Read-StatusSnapshot($Core,$Native,$Quality,$Controllers,$Frames){
 $row=[ordered]@{published=$Core.ReadUInt32(68);submitted=$Core.ReadUInt32(76);end_frames=$Core.ReadUInt32(2528);graphics_errors=$Core.ReadUInt32(80);xr_errors=$Core.ReadUInt32(84);source_fence_faults=$Core.ReadUInt32(2532);resource_ready=[bool]$Core.ReadInt32(36);renderer=$Core.ReadInt32(96);sample=[uint64]0;sampled_ms=[uint64]0;tracking_valid=$false;focused=$false;right_valid=$false;right_active=$false;tracking_read=$false;native_read=$false}
 for($attempt=0;$attempt -lt 8;$attempt++){
  $sequence=$Core.ReadInt32(88);if($sequence -band 1){continue}
  $row.sample=$Core.ReadUInt64(1368);$row.sampled_ms=$Core.ReadUInt64(1376);$row.tracking_valid=[bool]$Core.ReadInt32(1544);$row.focused=[bool]$Core.ReadInt32(1548);$row.right_valid=[bool]$Core.ReadInt32(1536);$row.right_active=[bool]$Core.ReadInt32(1540)
  [Threading.Thread]::MemoryBarrier();if($sequence -eq $Core.ReadInt32(88)){$row.tracking_read=$true;break}
 }
 if($Native){
  for($attempt=0;$attempt -lt 8;$attempt++){
   $sequence=$Native.ReadInt32(28);if($sequence -band 1){continue}
   $model=New-Object byte[] 64;[void]$Native.ReadArray(148,$model,0,64)
   $row.health=$Native.ReadInt32(48);$row.team=$Native.ReadInt32(52);$row.life=$Native.ReadUInt32(56);$row.hand_active=[bool]$Native.ReadUInt32(72);$row.hand_reason=$Native.ReadUInt32(76);$row.hand_frame=$Native.ReadUInt64(80);$row.model=[Text.Encoding]::ASCII.GetString($model).Split([char]0)[0]
   [Threading.Thread]::MemoryBarrier();if($sequence -eq $Native.ReadInt32(28)){$row.native_read=$true;break}
  }
 }
 if($Quality){$row.image_quality=[ordered]@{ready=[bool]$Quality.ReadUInt32(32);errors=$Quality.ReadUInt32(36);passes=$Quality.ReadUInt64(40);width=$Quality.ReadUInt32(48);height=$Quality.ReadUInt32(52);enabled=[bool]$Quality.ReadInt32(28);fxaa=[bool]$Quality.ReadInt32(60);sharpness=$Quality.ReadSingle(56)}}
 if($Controllers){
  $row.controllers=[ordered]@{features=$Controllers.ReadUInt32(24);haptic_applied=$Controllers.ReadUInt32(160);haptic_errors=$Controllers.ReadUInt32(164);haptic_dropped=$Controllers.ReadUInt32(168);haptic_gain=$Controllers.ReadSingle(180);snap_degrees=$Controllers.ReadUInt32(184);game_ui_open=[bool]$Controllers.ReadInt32(196);input_read=$false;action_mask=0;physical_feel='NOT_INFERRED'}
  for($attempt=0;$attempt -lt 8;$attempt++){
   $sequence=$Controllers.ReadInt32(20);if($sequence -band 1){continue}
   $row.controllers.action_mask=$Controllers.ReadUInt32(104)
   $row.controllers.inputs=[ordered]@{sample=$Controllers.ReadUInt64(40);epoch=$Controllers.ReadUInt64(48);valid=[bool]$Controllers.ReadUInt32(56);focused=[bool]$Controllers.ReadUInt32(60);trigger=$Controllers.ReadSingle(64);left_trigger=$Controllers.ReadSingle(72);left_grip=$Controllers.ReadSingle(76);right_grip=$Controllers.ReadSingle(80);left_x=$Controllers.ReadSingle(84);left_y=$Controllers.ReadSingle(88);right_x=$Controllers.ReadSingle(92);right_y=$Controllers.ReadSingle(96);buttons=$Controllers.ReadUInt32(100)}
   [Threading.Thread]::MemoryBarrier();if($sequence -eq $Controllers.ReadInt32(20)){$row.controllers.input_read=$true;break}
  }
 }
 if($Frames){$row.fresh_world_uploads=$Frames.ReadUInt64(32);$row.reused_world_frames=$Frames.ReadUInt64(40);$row.fps_policy=$Frames.ReadUInt32(20)}
 return [pscustomobject]$row
}
try{
 if(-not $GamePid){
  if(-not $SessionFile){$SessionFile=Join-Path $lab 'CURRENT_SESSION.json'}
  $session=Get-Content -LiteralPath $SessionFile -Raw|ConvertFrom-Json
  if($session.stage -ne 'running'){$output.stage='CLOSED';$output.message='The game session is closed.';throw 'NOVA_STATE_FINISHED'}
  $descriptor=Get-Content -LiteralPath (Join-Path $session.session 'game-control.json') -Raw|ConvertFrom-Json
  $GamePid=[int]$descriptor.pid
 }
 $output.pid=$GamePid;$process=Get-Process -Id $GamePid -ErrorAction SilentlyContinue
 if(-not $process){$output.stage='CLOSED';$output.message='The selected game process has exited.';throw 'NOVA_STATE_FINISHED'}
 if($process.ProcessName -ne 'csgo'){throw 'Selected process is not CS:GO.'}
 if($descriptor -and ($process.Path -ine $descriptor.executable -or [Math]::Abs(($process.StartTime.ToUniversalTime()-[DateTime]::Parse($descriptor.started_utc).ToUniversalTime()).TotalSeconds) -gt 1)){throw 'The recorded game process was replaced.'}
 $core=Open-StatusMap ('Local\NOVA_R3_SCENE_'+$GamePid) 2536
 if($core.ReadUInt32(0) -ne 0x41564f4e -or $core.ReadUInt32(4) -ne 3 -or $core.ReadUInt32(8) -ne 2536 -or $core.ReadUInt32(12) -ne $GamePid){throw 'Owned scene ABI/owner mismatch.'}
 $hostPidValue=$core.ReadUInt32(16);$xrHost=Get-Process -Id $hostPidValue -ErrorAction SilentlyContinue
 if(-not $xrHost -or $xrHost.ProcessName -notin @('nova_xr64_palm','nova_xr64_sim_palm')){throw 'Owned XR host is unavailable.'}
 $output.host_pid=$hostPidValue;$output.eye_width=$core.ReadUInt32(20);$output.eye_height=$core.ReadUInt32(24)
 $native=$null;$quality=$null;$controllers=$null;$frames=$null
 try{$frames=Open-StatusMap ('Local\NOVA_FPS_'+$GamePid) 64}catch{}
 if($frames -and ($frames.ReadUInt32(0) -ne 0x5046564e -or $frames.ReadUInt32(4) -ne 1 -or $frames.ReadUInt32(8) -ne 64 -or $frames.ReadUInt32(12) -ne $GamePid -or $frames.ReadUInt32(16) -ne $hostPidValue)){throw 'Fresh-frame status owner/ABI mismatch.'}
 try{$native=Open-StatusMap ('Local\NOVA_GAME_CONTROL_'+$GamePid) 728}catch{}
 if($native -and ($native.ReadUInt32(0) -ne 0x4347564e -or $native.ReadUInt32(4) -ne 1 -or $native.ReadUInt32(8) -ne 728 -or $native.ReadUInt32(12) -ne $GamePid)){throw 'Native status owner/ABI mismatch.'}
 try{$quality=Open-StatusMap ('Local\NOVA_IMAGE_QUALITY_'+$GamePid) 64}catch{}
 if($quality -and ($quality.ReadUInt32(0) -ne 0x5149564e -or $quality.ReadUInt32(4) -ne 1 -or $quality.ReadUInt32(8) -ne 64 -or $quality.ReadUInt32(12) -ne $GamePid -or $quality.ReadUInt32(16) -ne $hostPidValue)){throw 'Image status owner/ABI mismatch.'}
 try{$controllers=Open-StatusMap ('Local\NOVA_XR_BUTTONS_'+$GamePid) 200}catch{}
 if($controllers -and ($controllers.ReadUInt32(0) -ne 0x4942564e -or $controllers.ReadUInt32(4) -ne 2 -or $controllers.ReadUInt32(8) -ne 200 -or $controllers.ReadUInt32(12) -ne $GamePid -or $controllers.ReadUInt32(16) -ne $hostPidValue)){throw 'Controller status owner/ABI mismatch.'}
 $timer=[Diagnostics.Stopwatch]::StartNew();$first=Read-StatusSnapshot $core $native $quality $controllers $frames;Start-Sleep -Milliseconds $ObserveMilliseconds;$last=Read-StatusSnapshot $core $native $quality $controllers $frames;$timer.Stop()
 # TickCount's low word is enough for a <=250ms freshness check, with modular
 # subtraction across its wrap. Advancing frame counters are required as well.
 $nowLow=[BitConverter]::ToUInt32([BitConverter]::GetBytes([Environment]::TickCount),0)
 $age=([long]$nowLow-([long]$last.sampled_ms%4294967296)+4294967296)%4294967296
 $output.probe_ms=$timer.ElapsedMilliseconds;$output.new_stereo_pairs=[long]$last.published-[long]$first.published;$output.new_xr_frames=[long]$last.end_frames-[long]$first.end_frames;$output.tracking_age_ms=$age;$output.observed=$last;if($frames){$output.new_fresh_world_pairs=[long]$last.fresh_world_uploads-[long]$first.fresh_world_uploads}
 if($last.graphics_errors -or $last.xr_errors -or $last.source_fence_faults){$output.stage='PIPELINE_ERROR';$output.message='The graphics/XR pipeline reported an error; inspect this run before playing.'}
 elseif(-not $last.resource_ready){$output.stage='WAIT_RUNTIME';$output.message='Waiting for the selected OpenXR runtime and graphics resources.'}
 elseif(-not $quality){$output.stage='WAIT_IMAGE_PASS';$output.message='Waiting for the XR image-quality pass.'}
 elseif($quality -and (-not $last.image_quality.ready -or $last.image_quality.errors)){$output.stage='IMAGE_QUALITY_ERROR';$output.message='The XR image pass is unavailable or reported an error.'}
 elseif(-not $last.sample -or -not $last.end_frames){$output.stage='WAIT_HEADSET_SESSION';$output.message='No XR frames yet. Put on the headset and activate the selected OpenXR runtime.'}
 elseif($last.image_quality.enabled -and -not $last.image_quality.passes -and $last.submitted){$output.stage='IMAGE_PASS_NOT_RUNNING';$output.message='Stereo is reaching XR, but the enabled image pass has not drawn any frames.'}
 elseif(-not $last.tracking_read -or -not $last.tracking_valid -or $age -gt 250){$output.stage='WAIT_TRACKING';$output.message='Waiting for fresh headset tracking.'}
 elseif(-not $last.focused){$output.stage='WAIT_FOCUS';$output.message='Return from the headset dashboard to CS:GO.'}
 elseif(-not $controllers -or ($last.controllers.features -band 31) -ne 31 -or -not $last.controllers.input_read -or ($last.controllers.action_mask -band 3) -ne 3){$output.stage='WAIT_CONTROLLERS';$output.message='Waiting for both controller action sets, sticks and haptics.'}
 elseif($last.controllers.haptic_errors){$output.stage='HAPTIC_ERROR';$output.message='The vibration API reported an error; inspect this run.'}
 elseif($last.controllers.game_ui_open){$output.stage='MENU_ACTIVE';$output.message='The native game menu is open in XR. The left menu button returns to play.'}
 elseif(-not $last.native_read -or -not $last.health -or $last.life){$output.stage='WAIT_PLAYER';$output.message='Waiting for the local player to spawn.'}
 elseif(-not $last.right_valid -or -not $last.right_active){$output.stage='WAIT_RIGHT_CONTROLLER';$output.message='Waiting for the right controller to be tracked.'}
 elseif(-not $last.hand_active){$output.stage='WAIT_HAND';$output.message='The current native hand has not been admitted yet.'}
 elseif($output.new_stereo_pairs -le 0 -or $output.new_xr_frames -le 0 -or ($frames -and $output.new_fresh_world_pairs -le 0)){$output.stage='WAIT_FRESH_FRAMES';$output.message='Waiting for new game stereo frames to reach XR.'}
 else{$output.stage='READY';$output.message='Fresh game stereo frames, tracking, the right hand and both controller action sets are active.'}
}catch{
 if($_.Exception.Message -ne 'NOVA_STATE_FINISHED'){$output.stage='STARTING_OR_UNAVAILABLE';$output.message=$_.Exception.Message}
}finally{for($i=$owned.Count-1;$i -ge 0;$i--){$owned[$i].Dispose()}}
$output|ConvertTo-Json -Depth 6 -Compress
