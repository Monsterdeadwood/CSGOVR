[CmdletBinding()]
param([Parameter(Mandatory=$true)][int]$GamePid,[Parameter(Mandatory=$true)][int]$XrPid,[Parameter(Mandatory=$true)][string]$ResultDirectory,[ValidateRange(500,5000)][int]$IntervalMilliseconds=1000)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$result=[IO.Path]::GetFullPath($ResultDirectory)
if(-not $result.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'RESULTS'))+[IO.Path]::DirectorySeparatorChar),[StringComparison]::OrdinalIgnoreCase)){throw 'Telemetry result path is outside this package.'}
$utf8=[Text.UTF8Encoding]::new($false)
$summary=[ordered]@{schema='nova.telemetry.summary.v1';started_utc=[DateTime]::UtcNow.ToString('o');game_pid=$GamePid;host_pid=$XrPid;interval_ms=$IntervalMilliseconds;scope='OWN_COUNTERS_AND_NATIVE_LOGS;NO_PHYSICAL_VISUAL_CERTIFICATE';samples=0;stage_samples=@{};maximum_errors=@{graphics=0;xr=0;fence=0;haptic=0};issues=@();fps_units='source_issue_fps/game_fps count GPU issues; fresh_world_fps counts new XR world uploads; xr_end_hz includes reused images';rotation='telemetry.ndjson + two older 4 MiB segments';gpu_scope='Device-wide NVIDIA counters, optional, every five seconds'}
$rates=New-Object 'System.Collections.Generic.List[double]';$xrRates=New-Object 'System.Collections.Generic.List[double]'
$freshRates=New-Object 'System.Collections.Generic.List[double]'
$timer=[Diagnostics.Stopwatch]::StartNew();$previous=$null;$lastGpu=$null;$nextGpu=0;$cpuPrevious=$null
function Save-Json([string]$path,$value){[IO.File]::WriteAllText($path,($value|ConvertTo-Json -Depth 10),$utf8)}
function Percentile($list,[double]$fraction){if(-not $list.Count){return $null};$sorted=@($list|Sort-Object);return [Math]::Round($sorted[[int][Math]::Floor(($sorted.Count-1)*$fraction)],2)}
function Save-Summary {
 $summary.play_fps_windows=@{count=$rates.Count;p10=(Percentile $rates .1);median=(Percentile $rates .5);p90=(Percentile $rates .9)}
 $summary.fresh_world_fps_windows=@{count=$freshRates.Count;p10=(Percentile $freshRates .1);median=(Percentile $freshRates .5);p90=(Percentile $freshRates .9)}
 $summary.xr_end_rate=@{count=$xrRates.Count;p10=(Percentile $xrRates .1);median=(Percentile $xrRates .5);p90=(Percentile $xrRates .9)}
 $summary.updated_utc=[DateTime]::UtcNow.ToString('o')
 Save-Json (Join-Path $result 'telemetry-summary.json') $summary
}
try{
 $identity=Get-CimInstance Win32_Process -Filter ('ProcessId='+$GamePid)
 if(-not $identity -or $identity.Name -ne 'csgo.exe' -or $identity.CommandLine -notmatch '(^|\s)-insecure(\s|$)'){throw 'Telemetry requires the owned offline CS:GO process.'}
 $game=Get-Process -Id $GamePid;$xrProcess=Get-Process -Id $XrPid
 if($xrProcess.ProcessName -notin @('nova_xr64_palm','nova_xr64_sim_palm')){throw 'Unexpected XR host.'}
 $null=$game.Handle;$null=$xrProcess.Handle
 $os=Get-CimInstance Win32_OperatingSystem;$cpu=Get-CimInstance Win32_Processor|Select-Object -First 1
 $gpu=@(Get-CimInstance Win32_VideoController|Select-Object Name,DriverVersion,AdapterRAM)
 $runtime=$null;try{$runtime=(Get-ItemProperty 'HKLM:/SOFTWARE/Khronos/OpenXR/1' -ErrorAction Stop).ActiveRuntime}catch{}
 Save-Json (Join-Path $result 'system.json') ([ordered]@{schema='nova.system.v1';os=$os.Caption;os_build=$os.BuildNumber;ram_bytes=[long]$os.TotalVisibleMemorySize*1024;cpu=$cpu.Name;logical_processors=[Environment]::ProcessorCount;gpu=$gpu;openxr_runtime=$runtime;collector='No machine/user name, serial, network, account, audio, or screenshots collected here'})
 $smi=$null
 foreach($candidate in @((Join-Path $env:SystemRoot 'System32/nvidia-smi.exe'),(Join-Path $env:ProgramFiles 'NVIDIA Corporation/NVSMI/nvidia-smi.exe'))){if(Test-Path -LiteralPath $candidate){$smi=$candidate;break}}
 while(-not $game.HasExited -and -not $xrProcess.HasExited -and -not (Test-Path -LiteralPath (Join-Path $result 'telemetry.stop'))){
  $iteration=$timer.ElapsedMilliseconds
  $state=& (Join-Path $PSScriptRoot 'Get-RunState.ps1') -GamePid $GamePid -ObserveMilliseconds 50|ConvertFrom-Json
  $at=$timer.ElapsedMilliseconds;$game.Refresh();$xrProcess.Refresh()
  $cpuMs=$game.TotalProcessorTime.TotalMilliseconds+$xrProcess.TotalProcessorTime.TotalMilliseconds
  $cpuPercent=$null;if($cpuPrevious -and $at -gt $cpuPrevious.at){$cpuPercent=[Math]::Round(100*($cpuMs-$cpuPrevious.ms)/($at-$cpuPrevious.at)/[Environment]::ProcessorCount,2)}
  $cpuPrevious=@{at=$at;ms=$cpuMs}
  $gameFps=$null;$xrHz=$null;$freshFps=$null;$reuseHz=$null
  if($state.observed -and $previous -and $at -gt $previous.at){
   $seconds=($at-$previous.at)/1000.0
   $dp=[long]$state.observed.published-$previous.pairs;$dx=[long]$state.observed.end_frames-$previous.xr
   if($null -ne $state.observed.fresh_world_uploads -and $null -ne $previous.fresh){
    $df=[long]$state.observed.fresh_world_uploads-$previous.fresh;$dr=[long]$state.observed.reused_world_frames-$previous.reused
    if($df -ge 0){$freshFps=[Math]::Round($df/$seconds,2)};if($dr -ge 0){$reuseHz=[Math]::Round($dr/$seconds,2)}
   }
   if($dp -ge 0){$gameFps=[Math]::Round($dp/$seconds,2)}
   if($dx -ge 0){$xrHz=[Math]::Round($dx/$seconds,2)}
   if($state.observed.health -gt 0 -and -not $state.observed.life -and -not $state.observed.controllers.game_ui_open -and $state.observed.focused -and $gameFps -gt 0){if($null -ne $freshFps){$freshRates.Add($freshFps);if($freshRates.Count -gt 7200){$freshRates.RemoveAt(0)}};$rates.Add($gameFps);if($rates.Count -gt 7200){$rates.RemoveAt(0)}}
   if($xrHz -gt 0){$xrRates.Add($xrHz);if($xrRates.Count -gt 7200){$xrRates.RemoveAt(0)}}
  }
  if($state.observed){
   $previous=@{at=$at;pairs=[long]$state.observed.published;xr=[long]$state.observed.end_frames;fresh=$state.observed.fresh_world_uploads;reused=$state.observed.reused_world_frames}
   $pairs=@{graphics=$state.observed.graphics_errors;xr=$state.observed.xr_errors;fence=$state.observed.source_fence_faults;haptic=$state.observed.controllers.haptic_errors}
   foreach($key in $pairs.Keys){if($pairs[$key] -gt $summary.maximum_errors[$key]){$summary.maximum_errors[$key]=$pairs[$key]}}
  }
  if($smi -and $at -ge $nextGpu){
   $nextGpu=$at+5000
   try{
    $values=@(& $smi '--query-gpu=index,utilization.gpu,memory.used,memory.total,temperature.gpu,power.draw,clocks.current.graphics' '--format=csv,noheader,nounits' 2>$null)
    if($LASTEXITCODE -eq 0){$lastGpu=[ordered]@{sample_ms=$at;csv_columns='index,utilization_percent,memory_used_mib,memory_total_mib,temperature_c,power_w,graphics_clock_mhz';rows=$values}}
   }catch{$smi=$null;$summary.issues+=('GPU_COUNTERS_UNAVAILABLE: '+$_.Exception.Message)}
  }
  $stage=[string]$state.stage;if(-not $summary.stage_samples.ContainsKey($stage)){$summary.stage_samples[$stage]=0};$summary.stage_samples[$stage]++
  $sample=[ordered]@{utc=[DateTime]::UtcNow.ToString('o');elapsed_ms=$at;game_fps=$gameFps;source_issue_fps=$gameFps;fresh_world_fps=$freshFps;reused_world_hz=$reuseHz;xr_end_hz=$xrHz;cpu_percent_system_capacity=$cpuPercent;game_working_set_bytes=$game.WorkingSet64;xr_working_set_bytes=$xrProcess.WorkingSet64;gpu=$lastGpu;state=$state}
  $path=Join-Path $result 'telemetry.ndjson'
  if((Test-Path -LiteralPath $path) -and (Get-Item -LiteralPath $path).Length -gt 4MB){
   $old=Join-Path $result 'telemetry.2.ndjson';if(Test-Path -LiteralPath $old){[IO.File]::Delete($old)}
   $one=Join-Path $result 'telemetry.1.ndjson';if(Test-Path -LiteralPath $one){[IO.File]::Move($one,$old)}
   [IO.File]::Move($path,$one)
  }
  [IO.File]::AppendAllText($path,(($sample|ConvertTo-Json -Depth 10 -Compress)+[Environment]::NewLine),$utf8)
  $summary.samples++;if($summary.samples%15 -eq 0){Save-Summary}
  $remaining=$IntervalMilliseconds-($timer.ElapsedMilliseconds-$iteration);if($remaining -gt 0){Start-Sleep -Milliseconds $remaining}
 }
 $summary.status='ENDED';$summary.reason=if($game.HasExited){'GAME_EXITED'}elseif($xrProcess.HasExited){'HOST_EXITED'}else{'LAUNCHER_STOP'}
}catch{
 if(($game -and $game.HasExited) -or ($xrProcess -and $xrProcess.HasExited)){$summary.status='ENDED';$summary.reason='PROCESS_EXIT_DURING_SAMPLE'}
 else{$summary.status='COLLECTOR_ERROR';$summary.issues+=$_.Exception.Message}
}
finally{$summary.ended_utc=[DateTime]::UtcNow.ToString('o');Save-Summary}
if($summary.status -eq 'COLLECTOR_ERROR'){exit 1}
