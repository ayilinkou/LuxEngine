@echo off
setlocal
rem Usage: run.bat [windows-debug|windows-release]
set "config=%~1"
if not defined config set "config=windows-debug"
if not "%~2"=="" goto usage
if "%config%"=="windows-debug" goto run
if "%config%"=="windows-release" goto run
goto usage

:run
pushd "%~dp0" || exit /b 1
if not exist "build\%config%\LuxEngine.exe" (
    echo Build the executable first with: build.bat %config% 1>&2
    popd
    exit /b 1
)
"build\%config%\LuxEngine.exe"
set "result=%errorlevel%"
popd
exit /b %result%

:usage
echo Usage: run.bat [windows-debug^|windows-release] 1>&2
exit /b 2
