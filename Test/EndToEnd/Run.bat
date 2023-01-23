@echo off
setlocal EnableDelayedExpansion

:handle_help_request
if /i "%~1" == "-?"     call :usage & exit /b 0
if /i "%~1" == "--help" call :usage & exit /b 0

set runAllTests=0
if /i "%~1" == "--all" (
	set runAllTests=1
	if not defined VC_PLATFORM set VC_PLATFORM=Win32
)

:verify_platform
if not defined VC_PLATFORM (
	echo ERROR: VC_PLATFORM not set. Run 'SetVars' first or set manually.
	exit /b 1
)

set passed=0
set failed=0

echo ------------------------------------------------------------
echo %VC_PLATFORM% core tests
echo ------------------------------------------------------------

echo Synchronous requests
powershell -File TestPrimitives.ps1 %VC_PLATFORM%
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo Advises
powershell -File TestAdvises.ps1 %VC_PLATFORM%
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

if %runAllTests% equ 0 goto :end

if /i "%VC_PLATFORM%" == "Win32" (set VC_PLATFORM=x64) else (set VC_PLATFORM=Win32)

echo ------------------------------------------------------------
echo %VC_PLATFORM% core tests
echo ------------------------------------------------------------

echo Synchronous requests
powershell -File TestPrimitives.ps1 %VC_PLATFORM%
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo Advises
powershell -File TestAdvises.ps1 %VC_PLATFORM%
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo ------------------------------------------------------------
echo 64-bit Server / 32-bit Client protocol compatibility tests
echo ------------------------------------------------------------

echo Synchronous requests
powershell -File TestPrimitives.ps1 x64 Win32
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo Advises
powershell -File TestAdvises.ps1 x64 Win32
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo ------------------------------------------------------------
echo 32-bit Server / 64-bit Client protocol compatibility tests
echo ------------------------------------------------------------

echo Synchronous requests
powershell -File TestPrimitives.ps1 Win32 x64
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo Advises
powershell -File TestAdvises.ps1 Win32 x64
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo ------------------------------------------------------------
echo JS client protocol compatibility tests
echo ------------------------------------------------------------

echo Win32
powershell -File TestJSClient.ps1 Win32
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo x64
powershell -File TestJSClient.ps1 x64
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

:end
echo ------------------------------------------------------------
echo Test Results: !passed! Passed !failed! Failed

:success
exit /b 0

rem ************************************************************
rem Functions
rem ************************************************************

:usage
echo.
echo Usage: %~n0 [--all]
echo.
goto :eof
