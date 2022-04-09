@echo off
setlocal EnableDelayedExpansion

where /q node
if !errorlevel! neq 0 goto :node_missing
where /q npm
if !errorlevel! neq 0 goto :node_missing

echo Installing dependencies...
call npm install netdde --silent
if !errorlevel! neq 0 (
    echo ERROR: Failed to install dependencies.
    exit /b !errorlevel!
)

echo Running test...
node test.js
if !errorlevel! neq 0 (
    echo ERROR: Test run failed.
    exit /b !errorlevel!
)

exit /b 0

:node_missing
echo ERROR: NodeJS not installed or on the PATH.
echo You can install it with 'choco install -y nodejs-lts'.
exit /b !errorlevel!
