[CmdletBinding()]
param([int]$GamePid=0,[ValidateRange(-1,1)][double]$HapticGain=-1,[ValidateSet(0,15,30,45)][int]$SnapDegrees=0)
$ErrorActionPreference='Stop'
if(-not $GamePid){$lab=Split-Path -Parent $PSScriptRoot;$session=Get-Content -LiteralPath (Join-Path $lab 'CURRENT_SESSION.json') -Raw|ConvertFrom-Json;if($session.stage -ne 'running'){throw 'No running NOVA session.'};$descriptor=Get-Content -LiteralPath (Join-Path $session.session 'game-control.json') -Raw|ConvertFrom-Json;$GamePid=[int]$descriptor.pid}
$game=Get-CimInstance Win32_Process -Filter ('ProcessId='+$GamePid)
if(-not $game -or $game.Name -ne 'csgo.exe' -or $game.CommandLine -notmatch '(^|\s)-insecure(\s|$)'){throw 'The selected offline game is not running.'}
$mapping=[IO.MemoryMappedFiles.MemoryMappedFile]::OpenExisting('Local\NOVA_XR_BUTTONS_'+$GamePid);$view=$mapping.CreateViewAccessor(0,200)
$mutex=[Threading.Mutex]::new($false,'Local\NOVA_CONTROLLERS_CLIENT_'+$GamePid);$locked=$false
try{
 if($view.ReadUInt32(0) -ne 0x4942564e -or $view.ReadUInt32(4) -ne 2 -or $view.ReadUInt32(8) -ne 200 -or $view.ReadUInt32(12) -ne $GamePid){throw 'Controller owner/ABI mismatch.'}
 $hostPidValue=$view.ReadUInt32(16);$hostProcess=Get-Process -Id $hostPidValue -ErrorAction Stop;if($hostProcess.ProcessName -notin @('nova_xr64_palm','nova_xr64_sim_palm')){throw 'Controller host is unavailable.'}
 if($HapticGain -ge 0 -or $SnapDegrees){
  $locked=$mutex.WaitOne(10000);if(-not $locked){throw 'Controller settings are busy.'}
  if($view.ReadInt32(172) -ne $view.ReadInt32(176)){throw 'Previous controller setting is still pending.'}
  if($HapticGain -ge 0){$view.Write(180,[single]$HapticGain)};if($SnapDegrees){$view.Write(184,[uint32]$SnapDegrees)}
  $request=$view.ReadInt32(172)+1;[Threading.Thread]::MemoryBarrier();$view.Write(172,[int]$request);$deadline=[DateTime]::UtcNow.AddSeconds(8)
  while($view.ReadInt32(176) -ne $request -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 10}
  if($view.ReadInt32(176) -ne $request){throw 'Host did not apply the controller setting.'}
 }
 [ordered]@{schema='nova.controllers.v2';pid=$GamePid;host_pid=$hostPidValue;features=$view.ReadUInt32(24);haptic_gain=$view.ReadSingle(180);snap_degrees=$view.ReadUInt32(184);haptic_applied=$view.ReadUInt32(160);haptic_errors=$view.ReadUInt32(164);haptic_dropped=$view.ReadUInt32(168);game_ui_open=[bool]$view.ReadInt32(196);ack=$view.ReadInt32(176);physical_feel='NOT_CERTIFIED_BY_API_SUCCESS'}|ConvertTo-Json -Compress
}finally{if($locked){$mutex.ReleaseMutex()};$mutex.Dispose();$view.Dispose();$mapping.Dispose()}
