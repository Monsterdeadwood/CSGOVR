$ErrorActionPreference='Stop'
$lab=Split-Path -Parent $PSScriptRoot
$manifest=Get-Content -LiteralPath (Join-Path $lab 'DIAGNOSTICS_SHA256.json') -Raw | ConvertFrom-Json
foreach($entry in $manifest.PSObject.Properties){
 $file=Join-Path $lab $entry.Name
 if(-not (Test-Path -LiteralPath $file -PathType Leaf)){throw ('DIAGNOSTIC_BINARY_MISSING:'+ $entry.Name)}
 $stream=[IO.File]::OpenRead($file);$sha=[Security.Cryptography.SHA256]::Create()
 try{$hash=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
 if($hash -ne [string]$entry.Value){throw ('DIAGNOSTIC_BINARY_HASH_MISMATCH:'+ $entry.Name)}
}
Write-Host '[NOVA XR LAB] Diagnostic binary hashes verified.'
