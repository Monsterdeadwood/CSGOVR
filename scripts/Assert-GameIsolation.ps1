# The user's test rule: no second game or separate game XR host during a run.
$ErrorActionPreference='Stop'
$busy=@(Get-CimInstance Win32_Process -ErrorAction Stop | Where-Object {
 $path=[string]$_.ExecutablePath
 $name=[string]$_.Name
 $runtimeNames=@('vrserver.exe','vrcompositor.exe','vrdashboard.exe','vrwebhelper.exe','vrmonitor.exe','vrstartup.exe','steamtours.exe','vrlink.exe')
 $steamVr=($name -in $runtimeNames) -and ($path -match '(?i)[\\/]steamapps[\\/]common[\\/]SteamVR[\\/]')
 $wmr=($name -in @('MixedRealityVRServer.exe','MixedRealityVRDriver.exe')) -and ($path -match '(?i)[\\/]steamapps[\\/]common[\\/]MixedRealityVRDriver[\\/]')
 -not ($steamVr -or $wmr) -and (
 ($name -eq 'csgo.exe') -or ($name -like 'nova_xr64*') -or
 ($path -like '*\steamapps\common\*.exe') -or
 ($path -like 'C:\GOG Games\*.exe' -and $path -notlike 'C:\GOG Games\VR Research Tools\*') -or
 ($name -match '^(AC[0-9]+VR|BF[0-9]+VR|FC[0-9]+VR|CSGOVR|InsurgencyVR|.*_XR_X64)'))
})
if($busy.Count){
 $details=($busy | ForEach-Object {$_.Name+' (PID '+$_.ProcessId+')'}) -join ', '
 throw ('GAME_ISOLATION_REQUIRED: '+$details)
}
