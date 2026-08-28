@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo  CSGOVR RC3 PRE-LIVE IMPLEMENTATION CLOSURE - COLD ONLY
echo ============================================================
echo  This does NOT launch CS:GO, HLAE, OpenXR, an injector, or a compiler.
echo  It only reads the exact RC3 ZIP and writes a separate RESULT ZIP.
echo.

set "PKG=%~1"
if defined PKG (
  powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0audit_rc3_preflight.ps1" -Package "%PKG%"
) else (
  powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0audit_rc3_preflight.ps1"
)

set "RC=%ERRORLEVEL%"
echo.
if "%RC%"=="0" (
  echo [DONE] Upload the generated CSGOVR_RC3_PRELIVE_COLD_RESULT_*.zip for semantic review.
) else (
  echo [FAIL] Cold audit exited with code %RC%.
  echo        If the ZIP was not auto-found, drag the exact RC3 ZIP onto this BAT.
)
echo.
pause
exit /b %RC%
