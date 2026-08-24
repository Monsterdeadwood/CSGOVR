@echo off
setlocal
cd /d "%~dp0"
echo ================================================================
echo  CSGOVR T0 COLD HLAE RT PRIMITIVE ORACLE v1.0.0
echo  READ-ONLY: DOES NOT LAUNCH OR PATCH CS:GO / HLAE
echo ================================================================
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0ONE_CLICK.ps1"
set "EC=%ERRORLEVEL%"
echo.
echo ExitCode=%EC%
if not "%EC%"=="0" echo [FAILED] COLD oracle did not complete.
if "%EC%"=="0" echo [DONE] Return CSGOVR_T0_HLAE_RT_PRIMITIVE_RESULT_*.zip
pause
exit /b %EC%
