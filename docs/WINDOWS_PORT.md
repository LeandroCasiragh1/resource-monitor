# Windows Port Guide

## Overview

The `windows-port` branch provides Windows-compatible implementations of the resource monitoring tools. However, the project has inherent Linux dependencies that limit functionality on Windows.

## Platform Support

### ✅ Fully Supported on Windows

- **Resource Profiler** (`resource-profiler.exe`)
  - Uses Windows API: `GetProcessTimes`, `GetProcessMemoryInfo`
  - Provides: User time, kernel time, working set memory metrics
  - Works on native Windows or WSL (Windows Subsystem for Linux)

### ❌ Linux-Only (Not Available on Windows)

- **Namespace Analyzer**
  - Requires: `/proc/*/ns/` symlinks (Linux kernel feature)
  - Windows stub provides informative error message
  - Requires: WSL2 or native Linux environment
- **Cgroup Manager**
  - Requires: `/sys/fs/cgroup/` filesystem (Linux kernel feature)
  - Windows stub provides informative error message
  - Requires: WSL2 or native Linux environment

## Building on Windows

### Prerequisites

- GCC (MinGW) installed and in PATH
  - Download: [MinGW-w64](https://www.mingw-w64.org/)
  - Or use: [MSYS2](https://www.msys2.org/)
- Windows API headers (included with MinGW)

### Build Command

```powershell
# From repository root
.\build.ps1
```

### Build Output

```
bin/resource-profiler.exe        ✓ Fully functional
bin/namespace-analyzer.exe       ⚠ Stub (informational only)
bin/cgroup-manager.exe           ⚠ Stub (informational only)
```

## Running on Windows

### Resource Profiler

```powershell
# Get metrics for PID 1234, sample every 1000ms for 10 samples, save to output.csv
.\bin\resource-profiler.exe 1234 1000 10 output.csv

# Output CSV format:
# timestamp,pid,user_time_ms,kernel_time_ms,working_set_bytes
```

### Namespace Analyzer & Cgroup Manager

```powershell
.\bin\namespace-analyzer.exe list 1234
# Output: This feature requires Linux kernel support

.\bin\cgroup-manager.exe read /cgroup/systemd
# Output: This feature requires Linux kernel support
```

## Full Functionality via WSL2

To access all features on Windows, use **WSL2 (Windows Subsystem for Linux 2)**:

### Setup

```powershell
# From Windows PowerShell (admin)
wsl --install -d Ubuntu
wsl --set-default-version 2
```

### Build & Run in WSL2

```bash
# Enter WSL2
wsl

# Clone and build (from within WSL)
cd /mnt/c/Users/leand/OneDrive/Área\ de\ Trabalho/RA3_5/resource-monitor
make clean && make

# Run all tools with full Linux feature support
./bin/resource-profiler 1 1000 5 output.csv
./bin/namespace-analyzer list 1
./bin/cgroup-manager read /sys/fs/cgroup
```

## Design Rationale

### Why This Approach?

1. **Windows API Differences**: Process monitoring APIs differ fundamentally from Linux

   - Windows: `GetProcessTimes`, `GetProcessMemoryInfo` via PID handles
   - Linux: Reading `/proc/<pid>/stat`, `/proc/stat` text files

2. **Linux-Specific Features**: Some project requirements are impossible on Windows

   - Namespaces: Only exist in Linux kernel (mnt, pid, net, ipc, uts, user, cgroup)
   - Cgroups: Only exist in `/sys/fs/cgroup/` on Linux
   - No Windows equivalent exists

3. **Practical Solution**: Provide working profiler + informative stubs for Linux-only features

## Code Organization

### Windows-Specific Files

```
src/resource_profiler_windows.c      # Windows implementation using Windows API
src/namespace_analyzer_windows.c     # Stub with informative errors
src/cgroup_manager_windows.c         # Stub with informative errors
build.ps1                            # PowerShell build script
```

### Shared Files (Used by Both)

```
include/resource_profiler.h         # Header (API compatible)
include/namespace.h                 # Header (API compatible)
include/cgroup.h                    # Header (API compatible)
src/*_main.c                        # Main programs (platform-agnostic)
```

## Building Linux Version

To rebuild Linux versions (requires Linux or WSL):

```bash
# Clean previous builds
make clean

# Build with Linux implementations
make

# Result: Binaries with full functionality
./bin/resource-profiler
./bin/namespace-analyzer
./bin/cgroup-manager
```

## Limitations & Known Issues

### Windows Resource Profiler

- Timestamp precision: 100-nanosecond intervals (Windows FILETIME resolution)
- CPU% calculation requires system-wide stats (may need additional Windows API calls)
- Memory metrics: Working set only (doesn't include all virtual memory categories)

### Namespace Analyzer / Cgroup Manager

- These tools inform users they're Linux-only rather than silently failing
- Recommendation: Use WSL2 for complete functionality

## Performance Notes

Windows implementation may be slightly slower due to:

- Windows API call overhead vs. `/proc` filesystem reading
- Process handle management (OpenProcess/CloseProcess per sample)

For development: Consider WSL2 for accurate performance measurements.

## Contributing

When adding features:

1. Keep Linux version (`resource_profiler.c`) as primary
2. Add Windows equivalent (`resource_profiler_windows.c`) if feasible
3. Update both headers if changing public API
4. Document any platform-specific limitations
5. Maintain parity in error handling

## See Also

- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall design
- [USAGE.md](USAGE.md) - General usage guide
- [README.md](../README.md) - Project overview
