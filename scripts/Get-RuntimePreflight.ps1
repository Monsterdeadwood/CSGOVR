[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$OutputFile,[string]$Action='')
$ErrorActionPreference='Stop'
$report=[ordered]@{schema='nova.runtime-preflight.v1';utc=[DateTime]::UtcNow.ToString('o');action=$Action;process_64bit=[Environment]::Is64BitProcess;windows_version=[Environment]::OSVersion.Version.ToString();runtime_manifest=$null;runtime_library=$null;manifest_exists=$false;library_exists=$false;gpu=@();errors=@();scope='Read-only system runtime registration and display adapter information; actual runtime initialization is recorded by the host.'}
$hklm=$null;$key=$null
try{
 $hklm=[Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::LocalMachine,[Microsoft.Win32.RegistryView]::Registry64)
 $key=$hklm.OpenSubKey('SOFTWARE\Khronos\OpenXR\1')
 if($key){$report.runtime_manifest=[string]$key.GetValue('ActiveRuntime')}
 if($report.runtime_manifest){
  $report.manifest_exists=Test-Path -LiteralPath $report.runtime_manifest -PathType Leaf
  if($report.manifest_exists){
   $manifest=Get-Content -LiteralPath $report.runtime_manifest -Raw -Encoding UTF8|ConvertFrom-Json
   $library=[string]$manifest.runtime.library_path
   if($library){if(-not [IO.Path]::IsPathRooted($library)){$library=Join-Path (Split-Path -Parent $report.runtime_manifest) $library};$report.runtime_library=[IO.Path]::GetFullPath($library);$report.library_exists=Test-Path -LiteralPath $report.runtime_library -PathType Leaf}
  }
 }
}catch{$report.errors+=$_.Exception.Message}finally{if($key){$key.Dispose()};if($hklm){$hklm.Dispose()}}
try{$report.gpu=@(Get-CimInstance Win32_VideoController -ErrorAction Stop|Select-Object Name,DriverVersion,AdapterRAM)}catch{$report.errors+=$_.Exception.Message}
[void](New-Item -ItemType Directory -Path (Split-Path -Parent $OutputFile) -Force)
[IO.File]::WriteAllText($OutputFile,($report|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
