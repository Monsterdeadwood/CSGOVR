[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$SessionFile,[int]$TimeoutSeconds=110)
$ErrorActionPreference='Stop'
$deadline=[DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
$lab=Split-Path -Parent $PSScriptRoot
$firstWorld=$null;$captured=0
while([DateTime]::UtcNow -lt $deadline -and $captured -lt 3){
 try{
  $s=Get-Content -LiteralPath $SessionFile -Raw | ConvertFrom-Json
  if($s.stage -ne 'running'){break}
  $path=Join-Path $s.sim_directory 'state.json'
  if(Test-Path -LiteralPath $path){
   $state=Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
   if($state.projectionViews -eq 2 -and $state.sessionRunning){
    if($null -eq $firstWorld){$firstWorld=[DateTime]::UtcNow}
    $age=([DateTime]::UtcNow-$firstWorld).TotalSeconds
    $due=@(0,8,20)[$captured]
    if($age -ge $due){
     & (Join-Path $lab 'scripts/Control-XR.ps1') -SessionFile $SessionFile -Capture -Tag ('auto_world_'+($captured+1)) | Out-Null
     $captured++
    }
   }elseif($state.frame -gt 0 -and -not $state.sessionRunning){break}
  }
 }catch{Write-Host ('[XR OBSERVER] '+$_.Exception.Message)}
 Start-Sleep -Milliseconds 200
}
Write-Host ('[XR OBSERVER] saved stereo captures='+$captured)
