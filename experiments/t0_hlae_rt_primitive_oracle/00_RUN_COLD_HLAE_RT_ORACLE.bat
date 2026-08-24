@echo off
setlocal EnableExtensions
cd /d "%~dp0"
echo ================================================================
echo  CSGOVR T0 COLD HLAE RT PRIMITIVE ORACLE v1.1.1
echo  READ-ONLY: DOES NOT LAUNCH OR PATCH CS:GO / HLAE
echo ================================================================
set "CSGOVR_PS1=%~dp0ONE_CLICK.ps1"
if not exist "%CSGOVR_PS1%" (
  echo [FATAL] ONE_CLICK.ps1 missing.
  pause
  exit /b 89
)
echo [GATE] Windows PowerShell parser
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$tokens=$null;$errors=$null;[void][System.Management.Automation.Language.Parser]::ParseFile($env:CSGOVR_PS1,[ref]$tokens,[ref]$errors); if($errors.Count -gt 0){foreach($e in $errors){Write-Host ('[PARSER] ' + $e.Message + ' @ line ' + $e.Extent.StartLineNumber + ', column ' + $e.Extent.StartColumnNumber) -ForegroundColor Red}; exit 90}; Write-Host '[OK] ONE_CLICK.ps1 parser clean' -ForegroundColor Green"
set "PARSER_EC=%ERRORLEVEL%"
if not "%PARSER_EC%"=="0" (
  echo [FAILED] PowerShell parser gate exit code %PARSER_EC%.
  pause
  exit /b %PARSER_EC%
)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%CSGOVR_PS1%"
set "EC=%ERRORLEVEL%"
echo.
echo ExitCode=%EC%
if not "%EC%"=="0" echo [FAILED] COLD oracle did not complete.
if "%EC%"=="0" echo [DONE] Return CSGOVR_T0_HLAE_RT_PRIMITIVE_RESULT_*.zip
pause
exit /b %EC%
