@echo off
setlocal
cd /d "%~dp0"
set "PSModulePath=%SystemRoot%\System32\WindowsPowerShell\v1.0\Modules;%ProgramFiles%\WindowsPowerShell\Modules"
set "mode=Menu"
set "bound=0"
if /I "%~1"=="--check" set "mode=Check"
if /I "%~1"=="--meta" set "mode=Meta"
if /I "%~1"=="--fast" set "mode=Fast"
if /I "%~1"=="--test" set "mode=Sim"
if /I "%~1"=="--test" set "bound=600"
if /I "%~1"=="--report" set "mode=Report"
if /I "%~1"=="--stop" set "mode=Stop"
if /I "%~1"=="--status" set "mode=Status"
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\Portable-Menu.ps1" -Action %mode% -MaxSeconds %bound%
exit /b %errorlevel%
