@echo off
setlocal EnableDelayedExpansion

set passed=0
set failed=0

powershell -File TestPrimitives.ps1
if !errorlevel! equ 0 (set /a passed=!passed! + 1) else (set /a failed=!failed! + 1)

echo Test Results: !passed! Passed !failed! Failed
