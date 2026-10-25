@echo off

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%i"
if not defined VSPATH (
    echo Could not locate Visual Studio with the C++ toolset.
    exit /b 1
)

set "VCVER="
set /p VCVER=<"%VSPATH%\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt"

if /i "%VCToolsVersion%"=="%VCVER%" (
    exit /b 0
)

set "VSCMD_SKIP_SENDTELEMETRY=1"

if defined VCVER (
    call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" -vcvars_ver=%VCVER%
) else (
    call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat"
)
if errorlevel 1 exit /b %errorlevel%

exit /b 0
