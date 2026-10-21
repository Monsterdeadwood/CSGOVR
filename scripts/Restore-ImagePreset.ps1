[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$BackupDirectory)
$ErrorActionPreference='Stop'
$receiptPath=Join-Path $BackupDirectory 'receipt.json'
if(-not (Test-Path -LiteralPath $receiptPath)){return [pscustomobject]@{status='NO_PRESET_RECEIPT'}}
$r=Get-Content -LiteralPath $receiptPath -Raw -Encoding UTF8|ConvertFrom-Json
if(-not $r.changed){return [pscustomobject]@{status='NO_CHANGE_MADE'}}
if(Get-Process -Name csgo -ErrorAction SilentlyContinue){return [pscustomobject]@{status='DEFERRED_GAME_RUNNING'}}
$before=Join-Path $BackupDirectory 'video.before.txt'
if((Get-FileHash -LiteralPath $before -Algorithm SHA256).Hash.ToLowerInvariant() -ne $r.before_sha256){throw 'Original video backup hash mismatch.'}
if(-not (Test-Path -LiteralPath $r.path -PathType Leaf)){return [pscustomobject]@{status='CONFIG_MISSING_NOT_RECREATED'}}
if((Get-FileHash -LiteralPath $r.path -Algorithm SHA256).Hash.ToLowerInvariant() -eq $r.after_sha256){
 if($r.PSObject.Properties['original_file_existed'] -and -not $r.original_file_existed){
  Remove-Item -LiteralPath $r.path -Force
  $removed=[pscustomobject]@{schema='nova.video-restore.v2';status='RESTORED_ORIGINAL_ABSENCE';restored=@();preserved=@();original_backup=$before}
  $removed|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $BackupDirectory 'restore.json') -Encoding UTF8
  return $removed
 }
 [IO.File]::WriteAllBytes($r.path,[IO.File]::ReadAllBytes($before))
 $exact=[pscustomobject]@{schema='nova.video-restore.v1';status='RESTORED_EXACT_ORIGINAL';restored=@($r.settings.PSObject.Properties.Name);preserved=@();original_backup=$before}
 $exact|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $BackupDirectory 'restore.json') -Encoding UTF8
 return $exact
}
$current=[IO.File]::ReadAllText($r.path);$original=[IO.File]::ReadAllText($before)
$restored=New-Object 'System.Collections.Generic.List[string]';$preserved=New-Object 'System.Collections.Generic.List[string]'
foreach($property in $r.settings.PSObject.Properties){
 $name=$property.Name;$value=[string]$property.Value
 $pattern='(?m)^[ \t]*"(?:setting|setauto)\.'+[regex]::Escape($name)+'"[ \t]+"([^"\r\n]*)"[^\r\n]*\r?\n?'
 $actual=[regex]::Matches($current,$pattern)
 # Restore only values that still equal this run's preset. Keep user/game edits.
 if($actual.Count -ne 1 -or $actual[0].Groups[1].Value -ne $value){$preserved.Add($name);continue}
 $old=[regex]::Matches($original,$pattern);$current=[regex]::Replace($current,$pattern,'')
 $position=$current.LastIndexOf('}');if($position -lt 0){throw 'Invalid current video config; no restore written.'}
 $entries=($old|ForEach-Object {$_.Value}) -join ''
 if($entries){$current=$current.Insert($position,$entries)}
 $restored.Add($name)
}
if($restored.Count){[IO.File]::WriteAllText($r.path,$current,[Text.UTF8Encoding]::new($false))}
$result=[pscustomobject]@{schema='nova.video-restore.v1';status=if($preserved.Count){'RESTORED_OWN_VALUES_PRESERVED_OTHER_EDITS'}else{'RESTORED_OWN_VALUES'};restored=@($restored);preserved=@($preserved);original_backup=$before}
$result|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $BackupDirectory 'restore.json') -Encoding UTF8
$result
