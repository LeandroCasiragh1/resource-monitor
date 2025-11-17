# 📁 Project File Inventory & Structure

## Complete Directory Tree

```
resource-monitor/
├── 📋 Configuration & Build
│   ├── Makefile                         # GNU Make build configuration
│   ├── build.ps1                        # PowerShell Windows build script
│   ├── .gitignore                       # Git exclusions
│   ├── LICENSE                          # Project license
│   └── CHANGELOG.md                     # Version history
│
├── 📚 Documentation (Root Level)
│   ├── README.md                        # Project overview
│   ├── PROJECT_STATUS.md                # Current session progress report
│   ├── PR_GUIDE.md                      # Pull request creation guide
│   ├── CONTRIBUTING.md                  # Contribution guidelines
│   └── WINDOWS_PORT_COMPLETION_REPORT.md # Windows port completion summary
│
├── 📖 Documentation (docs/)
│   ├── ARCHITECTURE.md                  # System design & data flow
│   ├── USAGE.md                         # Build & usage instructions
│   ├── EXPERIMENTS.md                   # Experiment template (5 required)
│   └── WINDOWS_PORT.md                  # Windows-specific comprehensive guide
│
├── 🔧 Source Code (src/)
│   │
│   ├── 📊 Resource Profiler
│   │   ├── resource_profiler.c          # Linux implementation
│   │   ├── resource_profiler_windows.c  # Windows implementation (NEW)
│   │   └── resource_profiler_main.c     # CLI entry point
│   │
│   ├── 🔗 Namespace Analyzer
│   │   ├── namespace_analyzer.c         # Linux implementation
│   │   ├── namespace_analyzer_windows.c # Windows stub (NEW)
│   │   └── namespace_analyzer_main.c    # CLI entry point
│   │
│   ├── 🎯 Cgroup Manager
│   │   ├── cgroup_manager.c             # Linux implementation
│   │   ├── cgroup_manager_windows.c     # Windows stub (NEW)
│   │   └── cgroup_manager_main.c        # CLI entry point
│   │
│   ├── ⏱️ Monitoring (Additional Components)
│   │   ├── cpu_monitor.c                # CPU monitoring utilities
│   │   ├── memory_monitor.c             # Memory monitoring utilities
│   │   ├── io_monitor.c                 # I/O monitoring utilities
│   │   └── main.c                       # Alternative main program
│   │
│   └── 📦 Build Artifacts (generated)
│       ├── *.o                          # Object files (compiled)
│       └── *.exe                        # Executable binaries (Windows)
│
├── 📜 Headers (include/)
│   ├── resource_profiler.h              # Resource profiler API
│   ├── namespace.h                      # Namespace analyzer API
│   ├── cgroup.h                         # Cgroup manager API
│   └── monitor.h                        # Monitoring utilities API
│
├── ✅ Tests (tests/)
│   ├── test_resource_profiler.c         # Resource profiler tests
│   ├── test_namespace.c                 # Namespace analyzer tests
│   ├── test_cpu.c                       # CPU monitoring tests
│   ├── test_memory.c                    # Memory monitoring tests
│   └── test_io.c                        # I/O monitoring tests
│
└── 🛠️ Scripts (scripts/)
    ├── compare_tools.sh                 # Tool comparison script
    └── visualize.py                     # Data visualization script

```

---

## File Statistics

### By Category

| Category                  | Count  | Examples                                       |
| ------------------------- | ------ | ---------------------------------------------- |
| Source Files (.c)         | 14     | resource_profiler.c, namespace_analyzer.c, etc |
| Header Files (.h)         | 4      | resource_profiler.h, namespace.h, etc          |
| Test Files (.c in tests/) | 5      | test_\*.c                                     |
| Documentation (.md)       | 10     | README.md, USAGE.md, ARCHITECTURE.md           |
| Build Files               | 2      | Makefile, build.ps1                            |
| Scripts (.sh, .py)        | 2      | compare_tools.sh, visualize.py                 |
| License & Metadata        | 3      | LICENSE, .gitignore, CHANGELOG.md              |
| **TOTAL**                 | **40** |                                                |

### By Type

```
Total Lines of Code (approx):
- C Source: ~2000 lines
- C Headers: ~400 lines
- Documentation: ~3000+ lines
- Tests: ~300 lines
- Build Scripts: ~100 lines
━━━━━━━━━━━━━━━━━━━━━━━━━━━━
TOTAL: ~5800+ lines
```

---

## Key Files Reference

### Core Implementation

**Resource Profiler (CPU & Memory Monitoring)**

- `include/resource_profiler.h` - Public API
- `src/resource_profiler.c` - Linux implementation
- `src/resource_profiler_windows.c` - Windows implementation (Windows API)
- `src/resource_profiler_main.c` - Command-line interface

**Namespace Analyzer (Process Isolation)**

- `include/namespace.h` - Public API
- `src/namespace_analyzer.c` - Linux implementation (/proc/*/ns/ parsing)
- `src/namespace_analyzer_windows.c` - Windows stub (Linux-only feature)
- `src/namespace_analyzer_main.c` - Command-line interface

**Cgroup Manager (Resource Limits)**

- `include/cgroup.h` - Public API
- `src/cgroup_manager.c` - Linux implementation (/sys/fs/cgroup/ management)
- `src/cgroup_manager_windows.c` - Windows stub (Linux-only feature)
- `src/cgroup_manager_main.c` - Command-line interface

**Monitoring Utilities**

- `src/cpu_monitor.c` - CPU monitoring helpers
- `src/memory_monitor.c` - Memory monitoring helpers
- `src/io_monitor.c` - I/O monitoring helpers
- `include/monitor.h` - Shared API

### Build & Deployment

- `Makefile` - Linux/Unix build system

  - Targets: all, tests, install, check, clean
  - Compiles to: bin/resource-profiler, bin/namespace-analyzer, bin/cgroup-manager

- `build.ps1` - Windows build script
  - PowerShell-based compilation
  - Uses GCC/MinGW
  - Creates: bin\*.exe binaries

### Documentation

**Essential Guides**

- `README.md` - Project overview & quick start
- `docs/USAGE.md` - Detailed build & usage instructions
- `docs/ARCHITECTURE.md` - System design & implementation details

**Extended Documentation**

- `docs/EXPERIMENTS.md` - Template for 5 required experiments
- `docs/WINDOWS_PORT.md` - Windows-specific implementation guide
- `CONTRIBUTING.md` - Development guidelines
- `CHANGELOG.md` - Version history & updates
- `PROJECT_STATUS.md` - Current progress report
- `WINDOWS_PORT_COMPLETION_REPORT.md` - Windows port completion summary
- `PR_GUIDE.md` - Pull request creation instructions

### Testing

- `tests/test_resource_profiler.c` - Profiler smoke tests
- `tests/test_namespace.c` - Namespace analyzer tests
- `tests/test_cpu.c` - CPU metric tests
- `tests/test_memory.c` - Memory metric tests
- `tests/test_io.c` - I/O metric tests

### Utilities

- `scripts/compare_tools.sh` - Compare different monitoring tools
- `scripts/visualize.py` - Visualize collected metrics

### Metadata

- `LICENSE` - Project license
- `.gitignore` - Git exclusions (binaries, objects, etc)
- `CHANGELOG.md` - Version history

---

## File Relationships

### Compilation Dependencies

```
Makefile / build.ps1
    ↓
Compiles each .c file to .o
    ↓
Links .o files with headers (.h)
    ↓
Produces binaries:
    - resource-profiler
    - namespace-analyzer
    - cgroup-manager
    - test_runner
```

### Header Inclusions

```
resource_profiler_main.c → includes → resource_profiler.h → uses → resource_profiler.c
namespace_analyzer_main.c → includes → namespace.h → uses → namespace_analyzer.c
cgroup_manager_main.c → includes → cgroup.h → uses → cgroup_manager.c
test_*.c → includes → *.h → links → *.c implementations
```

### Platform-Specific Selection

```
Linux Build (Makefile):
    resource_profiler.c + resource_profiler_windows.c
    → Uses: resource_profiler.c (Linux version)

Windows Build (build.ps1):
    resource_profiler.c + resource_profiler_windows.c
    → Uses: resource_profiler_windows.c (Windows version)
```

---

## New Files in windows-port Branch

**Source Files** (3 new):

- `src/resource_profiler_windows.c` - Windows API implementation
- `src/namespace_analyzer_windows.c` - Linux-only stub
- `src/cgroup_manager_windows.c` - Linux-only stub

**Build Files** (1 updated):

- `build.ps1` - Updated for Windows compilation

**Documentation** (3 new + updates):

- `docs/WINDOWS_PORT.md` - Windows guide
- `WINDOWS_PORT_COMPLETION_REPORT.md` - Completion summary
- `PR_GUIDE.md` - PR creation guide
- `PROJECT_STATUS.md` - Status report

---

## File Access Patterns

### For Development

**To Build on Linux:**

```bash
make                    # Uses Makefile
make install            # Installs binaries
make test              # Runs tests
make check             # Quality checks
```

**To Build on Windows:**

```powershell
.\build.ps1            # Uses PowerShell script
.\bin\resource-profiler.exe   # Run binary
```

**To Work on Specific Component:**

```bash
# Resource Profiler
edit include/resource_profiler.h
edit src/resource_profiler.c
edit src/resource_profiler_main.c

# Namespace Analyzer
edit include/namespace.h
edit src/namespace_analyzer.c
edit src/namespace_analyzer_main.c

# Cgroup Manager
edit include/cgroup.h
edit src/cgroup_manager.c
edit src/cgroup_manager_main.c
```

### For Documentation Review

```bash
# Project Overview
cat README.md

# Getting Started
cat docs/USAGE.md

# Architecture Deep Dive
cat docs/ARCHITECTURE.md

# Windows Development
cat docs/WINDOWS_PORT.md
cat WINDOWS_PORT_COMPLETION_REPORT.md

# PR Review Preparation
cat PR_GUIDE.md
```

---

## File Naming Conventions

### Source Files

- `<component>.c` - Implementation for component (Linux version)
- `<component>_windows.c` - Windows version (NEW)
- `<component>_main.c` - CLI entry point
- `<monitor>.c` - Monitoring utilities

### Headers

- `<component>.h` - Public API for component
- `monitor.h` - Shared monitoring utilities

### Tests

- `test_<component>.c` - Tests for component

### Documentation

- `<FEATURE>.md` - Feature documentation
- `README.md` - Project overview (always root)
- `USAGE.md` - Usage guide (in docs/)
- `ARCHITECTURE.md` - Architecture (in docs/)

### Build Files

- `Makefile` - Unix/Linux build
- `build.ps1` - Windows/PowerShell build
- `.gitignore` - Git exclusions

### Reports

- `<FEATURE>_COMPLETION_REPORT.md` - Feature completion
- `PROJECT_STATUS.md` - Overall status
- `CHANGELOG.md` - Version history

---

## Quick File Lookup

| **"I need to..."**               | **File(s)**                   |
| -------------------------------- | ----------------------------- |
| ...understand the project        | README.md                     |
| ...build on Linux                | Makefile                      |
| ...build on Windows              | build.ps1                     |
| ...use the resource profiler     | docs/USAGE.md                 |
| ...understand the design         | docs/ARCHITECTURE.md          |
| ...review Windows implementation | docs/WINDOWS_PORT.md          |
| ...contribute code               | CONTRIBUTING.md               |
| ...run tests                     | tests/*.* (use test_runner)  |
| ...check progress                | PROJECT_STATUS.md             |
| ...create a PR                   | PR_GUIDE.md                   |
| ...profile a process             | src/resource_profiler_main.c  |
| ...analyze namespaces            | src/namespace_analyzer_main.c |
| ...manage cgroups                | src/cgroup_manager_main.c     |

---

## Total Project Metrics

| Metric                  | Value                     |
| ----------------------- | ------------------------- |
| **Total Files**         | 40+                       |
| **Source Code Files**   | 14                        |
| **Header Files**        | 4                         |
| **Test Files**          | 5                         |
| **Documentation Files** | 10                        |
| **Build Files**         | 2                         |
| **Configuration Files** | 2                         |
| **Lines of Code**       | ~5800+                    |
| **Lines of Docs**       | ~3000+                    |
| **Supported Platforms** | Linux, Windows (via APIs) |
| **Build Systems**       | 2 (Make, PowerShell)      |
| **Git Branches**        | 9                         |
| **Git Commits**         | 15+                       |

---

**Generated**: Current Session  
**Repository**: https://github.com/LeandroCasiragh1/resource-monitor  
**Last Updated**: windows-port branch  
**Status**: ✅ Complete and documented
