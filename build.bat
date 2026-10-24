@echo off
setlocal
rem Usage: build.bat [debug|release]
pushd "%~dp0" || exit /b 1
set "config=%~1"
if not defined config set "config=debug"

cmake --preset "%config%"
if errorlevel 1 goto done
cmake --build --preset "%config%" --parallel

:done
set "result=%errorlevel%"
popd
exit /b %result%
