@echo off
setlocal EnableDelayedExpansion

if not exist "Win32" mkdir "Win32" || exit /b
git clone https://github.com/chrisoldwood/NetDDE.git "Win32\NetDDE" || exit /b

if not exist "Win32\Lib" mkdir "Win32\Lib" || exit /b
git clone https://github.com/chrisoldwood/Core.git "Win32\Lib\Core" || exit /b
git clone https://github.com/chrisoldwood/NCL.git "Win32\Lib\NCL" || exit /b
git clone https://github.com/chrisoldwood/WCL.git "Win32\Lib\WCL" || exit /b

git clone https://github.com/chrisoldwood/Scripts.git "Win32\Scripts" || exit /b
