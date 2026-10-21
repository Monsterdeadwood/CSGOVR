[CmdletBinding()]
param([Parameter(Mandatory=$true)][int]$HostPID,[int]$TimeoutSeconds=15)
$ErrorActionPreference='Stop'
$lab=Split-Path -Parent $PSScriptRoot
if(-not $env:XRSIM_DIR -or -not $env:NOVA_LAB_SESSION){throw 'XR Lab session environment is missing.'}
$statePath=Join-Path $env:XRSIM_DIR 'state.json'
$deadline=[DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
$state=$null
while([DateTime]::UtcNow -lt $deadline){
 $process=Get-Process -Id $HostPID -ErrorAction SilentlyContinue
 if(-not $process){throw 'Sim host exited before the diagnostic session became ready.'}
 try{
  $state=Get-Content -LiteralPath $statePath -Raw | ConvertFrom-Json
  if($state.pid -eq $HostPID -and $state.sessionRunning -and $state.graphics -eq 'd3d11'){break}
 }catch{}
 $state=$null
 Start-Sleep -Milliseconds 100
}
if(-not $state){throw 'Sim did not report a running D3D11 session before the startup deadline.'}
$lines=@('pace free 90','focus policy permissive','head pose 0 1.65 0 0 0 0','hands reset','hand r follow on','hand r offset 0.4 0.2 -0.25','hand r valid on','hand l valid on')
$applied=& (Join-Path $lab 'diagnostics/xr-sim/xrsim-cmd.ps1') -Dir $env:XRSIM_DIR -Lines $lines -Quiet
$receipt=[ordered]@{host_pid=$HostPID;cmd_seq=$applied.cmdSeq;frame=$applied.frame;commands=$lines;status='ACKNOWLEDGED';utc=[DateTime]::UtcNow.ToString('o')}
$receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $env:NOVA_LAB_SESSION 'startup-control.json') -Encoding UTF8
Write-Host ('[NOVA XR LAB] Sim commands acknowledged; frame '+$applied.frame+', cmdSeq '+$applied.cmdSeq)
