# Windows Process Lab - Killer and User

This is a Windows port of the Linux process management lab. It demonstrates process creation, termination, and environment variable usage on Windows.

## Build

### Prerequisites

You need a C++ compiler. Options:
- **MinGW-w64** (recommended for simplicity)
- **Visual Studio C++ Build Tools**
- **MSVC** (via Visual Studio)

### Building

**Option 1: Using Batch file (Command Prompt)**
```
build.bat
```

**Option 2: Using PowerShell**
```powershell
.\build.ps1
```

**Option 3: Using Make (if MinGW make is installed)**
```
make
```

**Option 4: Manual compilation**
```
g++ -std=c++17 -Wall -Wextra -O2 -o killer.exe killer.cpp -lkernel32
g++ -std=c++17 -Wall -Wextra -O2 -o user.exe user.cpp -lkernel32
g++ -std=c++17 -Wall -Wextra -O2 -o victim.exe victim.cpp -lkernel32
```

## Programs

### `victim.exe`
A simple program that outputs its Process ID (PID) and sleeps until killed.
- Takes an optional name argument: `victim.exe victim1`

### `killer.exe`
A process termination utility with multiple options:
- `--id <pid>`: Terminate process by ID
- `--name <procname>`: Find and terminate processes by name
- `PROC_TO_KILL` environment variable: Comma-separated list of process names to terminate

Example:
```
killer.exe --id 1234
killer.exe --name victim
set PROC_TO_KILL=victim1,victim2,victim3
killer.exe
```

### `user.exe`
Demonstrates the complete workflow:
1. Sets `PROC_TO_KILL` environment variable
2. Spawns 10 victim processes
3. Launches killer with `--name` and `--id` options
4. Monitors which processes were killed
5. Clears the environment variable

## Key Differences from Linux Version

| Aspect | Linux | Windows |
|--------|-------|---------|
| Process enumeration | `/proc` filesystem | Windows API `CreateToolhelp32Snapshot` |
| Process termination | `kill()` signal | `TerminateProcess()` |
| Process properties | `prctl()` for name | Process name from process list |
| Wait for process | `waitpid()` | `WaitForSingleObject()` |
| Thread sleep | `usleep()` | `std::this_thread::sleep_for()` |
| Environment variables | `setenv()` | `SetEnvironmentVariableA()` |
| API headers | POSIX headers | `<windows.h>` |

## Running

1. Build the project (see Build section above)
2. Run `user.exe`:
```
user.exe
```

This will start the demonstration, showing process creation and termination logs.

## Notes

- On Windows, executable names often have the `.exe` extension, which the killer strips for comparison
- Process names are extracted from the process snapshot, not from /proc filesystem
- The implementation uses Windows API directly instead of POSIX calls
- Some Unix-specific features (like setting process display name with `prctl`) are not available on Windows

## Troubleshooting

- **"command not found"**: Ensure you have a C++ compiler installed and added to PATH
- **"Permission denied"**: You might need to run with administrator privileges to terminate certain processes
- **Compilation errors**: Make sure you're using C++17 or later

