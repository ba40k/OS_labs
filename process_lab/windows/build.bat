@echo off
REM Build script for Windows version of process_lab
REM Requires MinGW-w64 or Visual Studio C++ compiler

set CC=g++
set CFLAGS=-std=c++17 -Wall -Wextra -O2
set LDFLAGS=-lkernel32

echo Building killer.exe...
%CC% %CFLAGS% -o killer.exe killer.cpp %LDFLAGS%
if errorlevel 1 goto error

echo Building user.exe...
%CC% %CFLAGS% -o user.exe user.cpp %LDFLAGS%
if errorlevel 1 goto error

echo Building victim.exe...
%CC% %CFLAGS% -o victim.exe victim.cpp %LDFLAGS%
if errorlevel 1 goto error

echo.
echo All executables built successfully!
goto end

:error
echo Build failed!
exit /b 1

:end
