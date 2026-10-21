[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$SessionFile)
$ErrorActionPreference='Stop'
$lab=Split-Path -Parent $PSScriptRoot
$session=Get-Content -LiteralPath $SessionFile -Raw | ConvertFrom-Json
$descriptor=Get-Content -LiteralPath (Join-Path $session.session 'game-control.json') -Raw | ConvertFrom-Json
$receipt=[ordered]@{schema='nova.owned-session-stop.v1';game_pid=[int]$descriptor.pid;host_pid=0;actions=@();game_closed=$false;host_closed=$false}
$ownedGame=$null;$ownedHost=$null
function Get-OwnedProcess([int]$processId,[string[]]$allowedPaths,[datetime]$earliest,[datetime]$latest){
 $info=Get-CimInstance Win32_Process -Filter ('ProcessId='+$processId) -ErrorAction SilentlyContinue
 if(-not $info){return $null}
 if($info.ExecutablePath -notin $allowedPaths){throw ('Process path does not match this session: '+$processId)}
 $created=([datetime]$info.CreationDate).ToUniversalTime()
 if($created -lt $earliest -or $created -gt $latest){throw ('Process start does not match this session: '+$processId)}
 $held=[Diagnostics.Process]::GetProcessById($processId);$null=$held.Handle
 if([Math]::Abs(($held.StartTime.ToUniversalTime()-$created).TotalSeconds) -gt 1){$held.Dispose();throw 'PID changed before handle acquisition.'}
 return $held
}
try{
 $created=[datetime]::Parse($descriptor.started_utc).ToUniversalTime()
 $ownedGame=Get-OwnedProcess ([int]$descriptor.pid) @([string]$descriptor.executable) ($created.AddSeconds(-1)) ($created.AddSeconds(1))
 # Capture and validate the host identity BEFORE setting the shared stop flag.
 try{
  $mapping=[IO.MemoryMappedFiles.MemoryMappedFile]::OpenExisting('Local\NOVA_R3_SCENE_'+$descriptor.pid)
  try{$view=$mapping.CreateViewAccessor(0,2536);try{
   if($view.ReadUInt32(0) -ne 0x41564f4e -or $view.ReadUInt32(4) -ne 3 -or $view.ReadUInt32(8) -ne 2536 -or $view.ReadUInt32(12) -ne $descriptor.pid){throw 'Shared session identity mismatch.'}
   $receipt.host_pid=$view.ReadUInt32(16)
  }finally{$view.Dispose()}}finally{$mapping.Dispose()}
 }catch [IO.FileNotFoundException]{}
 if($receipt.host_pid){
  $ownedHost=Get-OwnedProcess ([int]$receipt.host_pid) @((Join-Path $lab 'bin\nova_xr64_sim_palm.exe'),(Join-Path $lab 'bin\nova_xr64_palm.exe')) ($created.AddSeconds(-1)) ([datetime]::UtcNow)
 }
 if($ownedGame -and -not $ownedGame.HasExited){
  try{
   $receipt.native_stop=(& (Join-Path $PSScriptRoot 'Control-Game.ps1') -SessionFile $SessionFile -StopGame -TimeoutSeconds 2 | ConvertFrom-Json)
   $receipt.actions+='native_graphics_thread_stop'
  }catch{$receipt.native_stop_error=$_.Exception.Message}
  # The command receipt acknowledges a request, not GPU retirement or exit.
  # Give the native graphics thread its next boundary before shared fallback.
  if(-not $ownedGame.HasExited){$null=$ownedGame.WaitForExit(6000)}
 }
 if(($ownedGame -and -not $ownedGame.HasExited) -or ($ownedHost -and -not $ownedHost.HasExited)){
  $start=New-Object Diagnostics.ProcessStartInfo
  $start.FileName=Join-Path $lab 'bin\nova_control32.exe';$start.UseShellExecute=$false;$start.CreateNoWindow=$true
  $start.EnvironmentVariables['NOVA_PID']=[string]$descriptor.pid
  $helper=[Diagnostics.Process]::Start($start)
  try{if(-not $helper.WaitForExit(3000)){throw 'Cooperative stop helper did not return.'};$receipt.shared_stop_exit=$helper.ExitCode;$receipt.actions+='shared_cooperative_stop'}finally{$helper.Dispose()}
 }
 if($ownedGame -and -not $ownedGame.WaitForExit(4000)){
  $receipt.window_close_requested=$ownedGame.CloseMainWindow();$receipt.actions+='owned_window_close'
  if(-not $ownedGame.WaitForExit(4000)){$ownedGame.Kill();$null=$ownedGame.WaitForExit(5000);$receipt.actions+='owned_game_forced_close'}
 }
 if($ownedHost -and -not $ownedHost.WaitForExit(8000)){
  $ownedHost.Kill();$null=$ownedHost.WaitForExit(5000);$receipt.actions+='owned_host_forced_close'
 }
 $receipt.game_closed=(-not $ownedGame -or $ownedGame.HasExited)
 $receipt.host_closed=(-not $ownedHost -or $ownedHost.HasExited)
 if($ownedGame -and $ownedGame.HasExited){$receipt.game_exit_code=$ownedGame.ExitCode}
 if($ownedHost -and $ownedHost.HasExited){$receipt.host_exit_code=$ownedHost.ExitCode}
 if(-not $receipt.game_closed -or -not $receipt.host_closed){throw 'Owned session processes have not both exited.'}
 $receipt | ConvertTo-Json -Depth 6 -Compress
}finally{if($ownedGame){$ownedGame.Dispose()};if($ownedHost){$ownedHost.Dispose()}}
