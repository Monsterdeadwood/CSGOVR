[CmdletBinding()]
param([string]$SteamDirectory='',[uint32]$SteamUserId=0)
$ErrorActionPreference='Stop'
if(-not $SteamDirectory){
 $SteamDirectory=Join-Path ${env:ProgramFiles(x86)} 'Steam'
 try{$value=[string](Get-ItemProperty 'HKCU:/Software/Valve/Steam' -ErrorAction Stop).SteamPath;if($value){$SteamDirectory=$value}}catch{}
}
if(-not $SteamUserId){try{$SteamUserId=[uint32](Get-ItemProperty 'HKCU:/Software/Valve/Steam/ActiveProcess' -ErrorAction Stop).ActiveUser}catch{}}
$accounts=@()
if($SteamUserId){$accounts=@((Join-Path $SteamDirectory ('userdata/'+$SteamUserId)))}
else{
 $accounts=@(Get-ChildItem -LiteralPath (Join-Path $SteamDirectory 'userdata') -Directory -ErrorAction SilentlyContinue|Where-Object {
  $_.Name -match '^[0-9]+$' -and ((Test-Path -LiteralPath (Join-Path $_.FullName '730/local/cfg/video.txt')) -or (Test-Path -LiteralPath (Join-Path $_.FullName '4465480/local/cfg/video.txt')))
 }|ForEach-Object {$_.FullName})
}
if($accounts.Count -ne 1){return [pscustomobject]@{status=if($accounts.Count){'STEAM_ACCOUNT_AMBIGUOUS'}else{'NO_EXISTING_VIDEO_CONFIG'};target='';source='';existed=$false}}
$target=Join-Path $accounts[0] '730/local/cfg/video.txt'
if(Test-Path -LiteralPath $target -PathType Leaf){return [pscustomobject]@{status='EXISTING_730';target=$target;source=$target;existed=$true}}
$source=Join-Path $accounts[0] '4465480/local/cfg/video.txt'
if(Test-Path -LiteralPath $source -PathType Leaf){return [pscustomobject]@{status='SEED_FROM_4465480';target=$target;source=$source;existed=$false}}
[pscustomobject]@{status='NO_EXISTING_VIDEO_CONFIG';target='';source='';existed=$false}
