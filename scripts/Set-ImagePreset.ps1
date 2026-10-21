[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$BackupDirectory,[string]$VideoConfigPath='',[string]$SteamDirectory='',[uint32]$SteamUserId=0)
$ErrorActionPreference='Stop'
if(Get-Process -Name csgo -ErrorAction SilentlyContinue){throw 'Apply the image preset before starting CS:GO.'}
[void](New-Item -ItemType Directory -Path $BackupDirectory -Force)
if(Test-Path -LiteralPath (Join-Path $BackupDirectory 'receipt.json')){throw 'PRESET_ALREADY_PREPARED: use a new result directory to preserve the original backup.'}
$source=$VideoConfigPath;$existed=$true;$selection='EXPLICIT'
if(-not $VideoConfigPath){
 $resolved=& (Join-Path $PSScriptRoot 'Resolve-VideoConfig.ps1') -SteamDirectory $SteamDirectory -SteamUserId $SteamUserId
 if(-not $resolved.target){
  $skipped=[pscustomobject]@{schema='nova.quality-video-preset.v2';status=('SKIPPED_'+$resolved.status);path=$null;changed=$false;settings=@{};effect='No user file changed; native launch commands still apply.'}
  $skipped|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $BackupDirectory 'receipt.json') -Encoding UTF8
  return $skipped
 }
 $VideoConfigPath=$resolved.target;$source=$resolved.source;$existed=$resolved.existed;$selection=$resolved.status
}
$VideoConfigPath=[IO.Path]::GetFullPath($VideoConfigPath)
$raw=[IO.File]::ReadAllBytes($source)
$before=Join-Path $BackupDirectory 'video.before.txt';[IO.File]::WriteAllBytes($before,$raw)
$text=[Text.Encoding]::UTF8.GetString($raw).TrimStart([char]0xfeff)
if($text -notmatch '(?is)^\s*"?(?:VideoConfig|config)"?\s*\{' -or $text.LastIndexOf('}') -lt 0){throw 'Invalid video config; nothing applied.'}
$settings=[ordered]@{mat_forceaniso='16';mat_antialias='0';mat_aaquality='0';mat_software_aa_strength='0';mat_motion_blur_enabled='0';mat_vsync='0';gpu_mem_level='2';gpu_level='3';cpu_level='2';csm_quality_level='3';mat_queue_mode='0';mat_texturestreaming='0'}
foreach($key in $settings.Keys){
 $pattern='(?m)^\s*"(?:setting|setauto)\.'+[regex]::Escape($key)+'"\s+"[^"\r\n]*"\r?\n?'
 $text=[regex]::Replace($text,$pattern,'')
 $position=$text.LastIndexOf('}');if($position -lt 0){throw 'Invalid video config.'}
 $text=$text.Insert($position,('`t"setting.'+$key+'"`t`t"'+$settings[$key]+'"`r`n').Replace('`t',"`t").Replace('`r`n',"`r`n"))
}
$bytes=[Text.UTF8Encoding]::new($false).GetBytes($text)
$changed=(-not $existed) -or ([Convert]::ToBase64String($raw) -ne [Convert]::ToBase64String($bytes))
$after=Join-Path $BackupDirectory 'video.after.txt';[IO.File]::WriteAllBytes($after,$bytes)
$receipt=[pscustomobject]@{schema='nova.quality-video-preset.v2';path=$VideoConfigPath;backup=$before;changed=$changed;original_file_existed=[bool]$existed;selection=$selection;seed_source=if($existed){''}else{$source};before_sha256=(Get-FileHash -LiteralPath $before -Algorithm SHA256).Hash.ToLowerInvariant();after_sha256=(Get-FileHash -LiteralPath $after -Algorithm SHA256).Hash.ToLowerInvariant();settings=$settings;effect='ENGINE_VALUES_REQUIRE_RUNTIME_READBACK'}
$receipt|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $BackupDirectory 'receipt.json') -Encoding UTF8
if($changed){
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant() -ne $receipt.before_sha256){throw 'Video config changed during preset preparation; nothing applied.'}
 if(-not $existed -and (Test-Path -LiteralPath $VideoConfigPath)){throw 'Video config appeared during preparation; nothing applied.'}
 [void](New-Item -ItemType Directory -Path (Split-Path -Parent $VideoConfigPath) -Force)
 try{[IO.File]::WriteAllBytes($VideoConfigPath,$bytes)}catch{if($existed){[IO.File]::WriteAllBytes($VideoConfigPath,$raw)};throw}
}
$receipt
