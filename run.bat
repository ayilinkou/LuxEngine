@echo off
setlocal
rem Usage: run.bat [debug|release]
pushd "%~dp0" || exit /b 1
set "config=%~1"
if not defined config set "config=debug"

"build\%config%\LuxEngine.exe"
set "result=%errorlevel%"
popd
exit /b %result%
