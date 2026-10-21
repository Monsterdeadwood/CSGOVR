[CmdletBinding()]
param([string[]]$Command=@(),[switch]$Capture,[string]$Tag='',[string]$SessionFile='',[ValidateRange(0,5000)][int]$WaitAfterCommandMilliseconds=250)
$ErrorActionPreference='Stop'
$lab=Split-Path -Parent $PSScriptRoot
if(-not $Tag){$Tag='live_'+[DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_fff')}
if(-not $SessionFile){$SessionFile=Join-Path $lab 'CURRENT_SESSION.json'}
$session=Get-Content -LiteralPath $SessionFile -Raw | ConvertFrom-Json
if($session.runtime -ne 'Sim' -or $session.stage -ne 'running' -or -not $session.sim_directory){throw 'This control channel requires a running Sim session.'}
$state=Get-Content -LiteralPath (Join-Path $session.sim_directory 'state.json') -Raw | ConvertFrom-Json
$process=Get-Process -Id $state.pid -ErrorAction SilentlyContinue
if(-not $state.sessionRunning -or -not $process -or $process.ProcessName -notin @('nova_xr64_sim','nova_xr64_sim_measured','nova_xr64_sim_palm')){throw 'The recorded Sim session has ended; commands were not sent.'}
if($Tag -notmatch '^[a-zA-Z0-9_-]{1,64}$'){throw 'Capture tag must contain 1..64 letters, digits, hyphens or underscores.'}
$tools=Join-Path $lab 'diagnostics\xr-sim'
$mutex=[Threading.Mutex]::new($false,('Local\NOVA_XR_LAB_CONTROL_'+$state.pid))
$locked=$false
try{
 try{$locked=$mutex.WaitOne(15000)}catch [Threading.AbandonedMutexException]{$locked=$true}
 if(-not $locked){throw 'XR control is busy; no commands were sent.'}
 if($Command.Count){& (Join-Path $tools 'xrsim-cmd.ps1') -Dir $session.sim_directory -Lines $Command -Quiet | Out-Null;Start-Sleep -Milliseconds $WaitAfterCommandMilliseconds}
 if($Capture){& (Join-Path $tools 'xrsim-shot.ps1') -Dir $session.sim_directory -Out (Join-Path $session.session $Tag)}
 else {& (Join-Path $tools 'xrsim-state.ps1') -Dir $session.sim_directory -Quiet}
}finally{if($locked){$mutex.ReleaseMutex()};$mutex.Dispose()}
