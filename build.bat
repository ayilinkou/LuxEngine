@echo off
setlocal
rem Usage: build.bat [windows-debug|windows-release]
set "config=%~1"
if not defined config set "config=windows-debug"
if not "%~2"=="" goto usage
if "%config%"=="windows-debug" goto build
if "%config%"=="windows-release" goto build
goto usage

:build
pushd "%~dp0" || exit /b 1
call "%~dp0scripts\setup_env.bat"
if errorlevel 1 goto done

cmake --preset "%config%"
if errorlevel 1 goto done
cmake --build --preset "%config%" --parallel

:done
set "result=%errorlevel%"
popd
exit /b %result%

:usage
echo Usage: build.bat [windows-debug^|windows-release] 1>&2
exit /b 2
