[CmdletBinding()]
param([string]$Command='',[switch]$StopGame,[int]$GamePid=0,[string]$SessionFile='',[ValidateRange(1,30)][int]$TimeoutSeconds=8)
$ErrorActionPreference='Stop'
$descriptor=$null
if($StopGame){if($Command){throw 'Use either Command or StopGame.'};$Command='__NOVA_STOP_GAME__'}
$lab=Split-Path -Parent $PSScriptRoot
if(-not $GamePid){
 if(-not $SessionFile){$SessionFile=Join-Path $lab 'CURRENT_SESSION.json'}
 $session=Get-Content -LiteralPath $SessionFile -Raw | ConvertFrom-Json
 if($session.stage -ne 'running'){throw 'The selected game session has ended.'}
 $descriptor=Get-Content -LiteralPath (Join-Path $session.session 'game-control.json') -Raw | ConvertFrom-Json
 $GamePid=[int]$descriptor.pid
}
$game=Get-CimInstance Win32_Process -Filter ('ProcessId='+$GamePid) -ErrorAction Stop
if(-not $game -or $game.Name -ne 'csgo.exe' -or $game.CommandLine -notmatch '(^|\s)-insecure(\s|$)'){throw 'No selected offline CS:GO process.'}
if($descriptor){
 if($game.ExecutablePath -ine $descriptor.executable -or [Math]::Abs((([datetime]$game.CreationDate).ToUniversalTime()-[datetime]::Parse($descriptor.started_utc).ToUniversalTime()).TotalSeconds) -gt 1){throw 'The recorded game process was replaced.'}
}
$name='Local\NOVA_GAME_CONTROL_'+$GamePid
$mapping=[IO.MemoryMappedFiles.MemoryMappedFile]::OpenExisting($name)
$view=$mapping.CreateViewAccessor(0,728)
$mutex=[Threading.Mutex]::new($false,('Local\NOVA_GAME_CONTROL_CLIENT_'+$GamePid))
$locked=$false
try{
 if($view.ReadUInt32(0) -ne 0x4347564e -or $view.ReadUInt32(4) -ne 1 -or $view.ReadUInt32(8) -ne 728 -or $view.ReadUInt32(12) -ne $GamePid){throw 'Native console channel ABI/owner mismatch.'}
 $request=$null
 if($Command){
  if($Command -match '[^\x20-\x7e]' -or $Command.Length -gt 510){throw 'Console command must contain 1..510 printable ASCII characters.'}
  try{$locked=$mutex.WaitOne(10000)}catch [Threading.AbandonedMutexException]{$locked=$true}
  if(-not $locked){throw 'Console channel has another writer.'}
  if($view.ReadUInt32(16) -ne $view.ReadUInt32(20)){throw 'A previous console command is still pending.'}
  $payload=[Text.Encoding]::ASCII.GetBytes($Command)
  $request=$view.ReadUInt32(16)+1
  $view.WriteArray(212,$payload,0,$payload.Length)
  $view.Write(32,[uint32]$payload.Length)
  [Threading.Thread]::MemoryBarrier()
  $view.Write(16,[int]$request)
  $deadline=[DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
  while($view.ReadUInt32(20) -ne $request){if([DateTime]::UtcNow -ge $deadline){throw 'Native render thread did not acknowledge this command.'};Start-Sleep -Milliseconds 20}
  if($view.ReadInt32(24) -ne 0){throw ('Native queue rejected the command: '+$view.ReadInt32(24))}
  Start-Sleep -Milliseconds 80
 }
 $data=$null
 for($attempt=0;$attempt -lt 20;$attempt++){
  $before=$view.ReadUInt32(28);if($before -band 1){Start-Sleep -Milliseconds 5;continue}
  $model=New-Object byte[] 64;[void]$view.ReadArray(148,$model,0,64)
  $axes=@();for($i=0;$i -lt 9;$i++){$axes+=$view.ReadSingle(112+$i*4)}
  $data=[ordered]@{schema='nova.game-control.v1';pid=$GamePid;request=$request;acknowledged=$view.ReadUInt32(20);command=$Command;receipt_scope='QUEUED_ON_NATIVE_RENDER_THREAD';thread=$view.ReadUInt32(36);tick_ms=$view.ReadUInt64(40);health=$view.ReadInt32(48);team=$view.ReadInt32(52);life=$view.ReadUInt32(56);observer=$view.ReadInt32(60);checksum=$view.ReadUInt32(64);bones=$view.ReadUInt32(68);hand_active=$view.ReadUInt32(72);hand_reason=$view.ReadUInt32(76);hand_frame=$view.ReadUInt64(80);observed_palm=@($view.ReadSingle(88),$view.ReadSingle(92),$view.ReadSingle(96));target_palm=@($view.ReadSingle(100),$view.ReadSingle(104),$view.ReadSingle(108));observed_palm_axes=$axes;model=[Text.Encoding]::ASCII.GetString($model).Split([char]0)[0]}
  [Threading.Thread]::MemoryBarrier()
  if($before -eq $view.ReadUInt32(28)){break};$data=$null
 }
 if(-not $data){throw 'Could not read a stable native game state.'}
 $data | ConvertTo-Json -Depth 5 -Compress
}finally{if($locked){$mutex.ReleaseMutex()};$mutex.Dispose();$view.Dispose();$mapping.Dispose()}
