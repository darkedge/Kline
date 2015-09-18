@echo off

rem // Switch Configuration
if "%1" == ""Debug"" goto debug
if "%1" == ""Release"" goto release
echo "Unknown configuration!"
goto end

:debug
if "%2" == ""Win32"" goto debug_win32
if "%2" == ""x64"" goto debug_x64
goto :platform_error

:release
if "%2" == ""Win32"" goto release_win32
if "%2" == ""x64"" goto release_x64
goto :platform_error

:debug_win32
copy /y "%~dp0..\extern\FMOD\lib\fmodL.dll" %3
:copy /y "%~dp0..\extern\OpenCL\lib\Win32\OpenCL.dll" %3
goto end

:debug_x64
copy /y "%~dp0..\extern\FMOD\lib\fmodL64.dll" %3
:copy /y "%~dp0..\extern\OpenCL\lib\x64\OpenCL.dll" %3
goto end

:release_win32
copy /y "%~dp0..\extern\FMOD\lib\fmod.dll" %3
:copy /y "%~dp0..\extern\OpenCL\lib\Win32\OpenCL.dll" %3
goto end

:release_x64
copy /y "%~dp0..\extern\FMOD\lib\fmod64.dll" %3
:copy /y "%~dp0..\extern\OpenCL\lib\x64\OpenCL.dll" %3
goto end

:platform_error
echo "Unknown platform!"
goto end

:end
