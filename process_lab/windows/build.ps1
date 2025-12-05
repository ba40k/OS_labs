# Build script for Windows version of process_lab
# Requires MinGW-w64 or Visual Studio C++ compiler

$CC = "g++"
$CFLAGS = "-std=c++17 -Wall -Wextra -O2"
$LDFLAGS = "-lkernel32"

Write-Host "Building killer.exe..."
& $CC $CFLAGS -o killer.exe killer.cpp $LDFLAGS
if ($LASTEXITCODE -ne 0) { Write-Host "Build failed!"; exit 1 }

Write-Host "Building user.exe..."
& $CC $CFLAGS -o user.exe user.cpp $LDFLAGS
if ($LASTEXITCODE -ne 0) { Write-Host "Build failed!"; exit 1 }

Write-Host "Building victim.exe..."
& $CC $CFLAGS -o victim.exe victim.cpp $LDFLAGS
if ($LASTEXITCODE -ne 0) { Write-Host "Build failed!"; exit 1 }

Write-Host ""
Write-Host "All executables built successfully!"
