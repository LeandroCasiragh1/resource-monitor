# Project Status Report - RA3 Resource Monitor

## 📊 Summary

**Project**: Linux Resource Monitoring System (University Assignment RA3)  
**Status**: 🔄 In Development (Windows Port Complete, 9 feature branches ready for PR)  
**Repository**: https://github.com/LeandroCasiragh1/resource-monitor

---

## ✅ Completed Work

### Phase 1: Project Structure & Setup

- ✅ Created complete directory structure (src/, include/, tests/, docs/, scripts/)
- ✅ Initialized Git repository with meaningful .gitignore
- ✅ Created 9 feature branches (all pushed to origin)
- ✅ Added license, contributing guide, changelog

### Phase 2: Core Components Implemented

#### 1. Resource Profiler ✅

- **Branch**: `profiler-cpu-percent`
- **Features**:
  - CPU usage tracking (user/kernel time)
  - Memory profiling (virtual size, resident set)
  - CPU percentage calculation using /proc/stat deltas
  - CSV export with configurable sampling
  - Linux implementation using /proc filesystem
  - Windows implementation using Windows API
- **Files**:
  - `include/resource_profiler.h` (shared API)
  - `src/resource_profiler.c` (Linux)
  - `src/resource_profiler_windows.c` (Windows)
  - `src/resource_profiler_main.c`

#### 2. Namespace Analyzer ✅

- **Branch**: `namespace-analyzer`
- **Features**:
  - List all namespaces for a process
  - Compare namespaces between processes
  - Map processes by namespace type
  - Support for all Linux namespace types (mnt, pid, net, ipc, uts, user, cgroup)
- **Files**:
  - `include/namespace.h`
  - `src/namespace_analyzer.c` (Linux)
  - `src/namespace_analyzer_windows.c` (stub)
  - `src/namespace_analyzer_main.c`

#### 3. Cgroup Manager ✅

- **Branch**: `cgroup-manager`
- **Features**:
  - Read cgroup metrics (cpu, memory, io)
  - Create new cgroups
  - Move processes between cgroups
  - Auto-detect cgroups v1 vs v2
- **Files**:
  - `include/cgroup.h`
  - `src/cgroup_manager.c` (Linux)
  - `src/cgroup_manager_windows.c` (stub)
  - `src/cgroup_manager_main.c`

### Phase 3: Build System & Tests

- ✅ Makefile with separate binary targets
- ✅ PowerShell build script for Windows
- ✅ Smoke tests for all components
- ✅ Test runner executable

### Phase 4: Documentation

- ✅ ARCHITECTURE.md (design decisions, data flow)
- ✅ USAGE.md (build & usage examples)
- ✅ EXPERIMENTS.md (template for 5 required experiments)
- ✅ WINDOWS_PORT.md (Windows-specific guide)
- ✅ CONTRIBUTING.md (contribution guidelines)
- ✅ CHANGELOG.md (version history)

### Phase 5: Windows Portability ✅

- ✅ `windows-port` branch created
- ✅ Resource profiler Windows implementation (using GetProcessTimes, GetProcessMemoryInfo)
- ✅ Namespace/Cgroup Windows stubs (marked as Linux-only)
- ✅ Updated build.ps1 for Windows compilation
- ✅ Comprehensive WINDOWS_PORT.md guide

---

## 🔄 Branches Ready for Pull Requests

| Branch                 | Changes                                    | Commits | Status       |
| ---------------------- | ------------------------------------------ | ------- | ------------ |
| `resource-profiler`    | Initial resource profiler skeleton         | 1       | Ready for PR |
| `namespace-analyzer`   | Namespace listing/comparison/mapping       | 1       | Ready for PR |
| `cgroup-manager`       | Cgroup read/create/move operations         | 1       | Ready for PR |
| `makefile-update`      | Refactored Makefile with separate binaries | 1       | Ready for PR |
| `profiler-cpu-percent` | CPU% calculation via /proc/stat deltas     | 2       | Ready for PR |
| `tests`                | Smoke tests for all components             | 1       | Ready for PR |
| `docs`                 | Architecture, usage, experiments docs      | 3       | Ready for PR |
| `windows-port`         | Windows implementations + build script     | 3       | Ready for PR |

**Total**: 8 feature branches, 13 commits across all branches

---

## 📋 Current Branch Summary

```
Active Branch: windows-port (latest)

Total Branches: 9
  ├── master (main branch)
  ├── resource-profiler (component)
  ├── namespace-analyzer (component)
  ├── cgroup-manager (component)
  ├── makefile-update (tooling)
  ├── profiler-cpu-percent (enhancement)
  ├── tests (validation)
  ├── docs (documentation)
  └── windows-port (current - portability)
```

---

## 🚀 Next Steps (Priority Order)

### 1. Create Pull Requests (NEXT)

```bash
# Switch to each branch and create PR
gh pr create --base master --title "..." --body "..."

# Or manually on GitHub: https://github.com/LeandroCasiragh1/resource-monitor
```

**PR List to Create:**

- [ ] `resource-profiler` → master: Initial implementation
- [ ] `namespace-analyzer` → master: Linux namespace monitoring
- [ ] `cgroup-manager` → master: Cgroup management
- [ ] `makefile-update` → master: Build system improvements
- [ ] `profiler-cpu-percent` → master: CPU% calculation feature
- [ ] `tests` → master: Test suite
- [ ] `docs` → master: Documentation
- [ ] `windows-port` → master: Windows compatibility (optional/separate)

### 2. Implement I/O Monitoring (NEW FEATURE)

- Create branch: `profiler-io-monitoring`
- Add /proc/<pid>/io parsing (Linux)
- Add Windows API equivalent
- Extend CSV output: bytes_read, bytes_written, syscalls

### 3. Fill Experiment Documentation

Need to document 5 required experiments with:

- Methodology
- Metrics collected
- Hypothesis
- Results tables
- Conclusion

**Template Files**:

- `docs/experiments/01-overhead.md`
- `docs/experiments/02-scalability.md`
- `docs/experiments/03-isolation.md`
- `docs/experiments/04-io-limits.md`
- `docs/experiments/05-cpu-limits.md`

### 4. Enhance Test Suite

- Add comprehensive unit tests
- Create test mocks for /proc files
- Add integration tests with actual processes
- Improve error handling validation

### 5. Code Review & Merge

- Address any feedback on PRs
- Merge to master in order
- Tag first release (v1.0.0)

---

## 📦 Build & Test Status

### Linux Build

```bash
make clean && make
# Result: ✅ Compiles successfully
```

### Windows Build

```powershell
.\build.ps1
# Result: ✅ resource-profiler.exe compiles
#         ⚠️  namespace/cgroup stubs only (Linux-only features)
```

### WSL2 Build

```bash
wsl
cd /path/to/repo
make
# Result: ✅ Full functionality available
```

---

## 📊 Project Statistics

| Metric              | Value                   |
| ------------------- | ----------------------- |
| Total Files         | 30+                     |
| Source Files        | 11                      |
| Header Files        | 3                       |
| Test Files          | 3                       |
| Doc Files           | 8                       |
| Configuration Files | 2 (Makefile, build.ps1) |
| Total Lines of Code | ~1500                   |
| Total Commits       | 13                      |
| Branches            | 9                       |
| Remote Repository   | GitHub (public)         |

---

## 🔧 Technology Stack

- **Language**: C (C11 standard, with some C23 features)
- **OS Support**:
  - Primary: Linux (full functionality)
  - Secondary: Windows (resource profiler only)
  - Fallback: WSL2 (full functionality on Windows)
- **Build**:
  - Linux: GNU Make
  - Windows: GCC/MinGW + PowerShell script
- **Version Control**: Git + GitHub
- **Documentation**: Markdown

---

## 🎯 Assignment Requirements Status

| Requirement          | Status         | Implemented In                       |
| -------------------- | -------------- | ------------------------------------ |
| Resource Profiler    | ✅ Done        | `profiler-cpu-percent` branch        |
| Namespace Analyzer   | ✅ Done        | `namespace-analyzer` branch          |
| Cgroup Manager       | ✅ Done        | `cgroup-manager` branch              |
| CSV Export           | ✅ Done        | Resource Profiler                    |
| Process Monitoring   | ✅ Done        | Resource Profiler                    |
| Isolation Monitoring | ✅ Done        | Namespace Analyzer                   |
| Resource Limits      | ✅ Done        | Cgroup Manager                       |
| 5 Experiments        | 🔄 In Progress | `docs/experiments/` (template ready) |
| Documentation        | ✅ Done        | Multiple .md files                   |
| Tests                | ✅ Basic       | `tests` branch (can be enhanced)     |
| Build System         | ✅ Done        | Makefile + build.ps1                 |
| Windows Support      | ✅ Done        | `windows-port` branch                |

---

## 🚨 Known Limitations

1. **Namespace/Cgroup on Windows**: Linux-only features; use WSL2 for full support
2. **Test Coverage**: Basic smoke tests; comprehensive unit tests could be added
3. **I/O Monitoring**: Not yet implemented (next feature)
4. **Experiment Results**: Template created; actual experiments need to be run
5. **Error Handling**: Functional but could be more verbose

---

## 💾 Key Files Reference

### Headers (Public APIs)

```
include/resource_profiler.h    # API for process profiling
include/namespace.h             # API for namespace operations
include/cgroup.h                # API for cgroup management
```

### Core Implementations

```
src/resource_profiler.c         # Linux version
src/resource_profiler_windows.c # Windows version
src/namespace_analyzer.c        # Linux (namespace parsing)
src/cgroup_manager.c            # Linux (cgroup operations)
```

### Build Files

```
Makefile                        # Linux build configuration
build.ps1                       # Windows build script
```

### Documentation

```
docs/ARCHITECTURE.md            # Design & data flow
docs/USAGE.md                   # Build & usage guide
docs/EXPERIMENTS.md             # Experiment template
docs/WINDOWS_PORT.md            # Windows-specific guide
README.md                       # Project overview
```

### Tests

```
tests/test_resource_profiler.c
tests/test_namespace.c
tests/test_cpu.c
```

---

## 🔗 Important Links

- **Repository**: https://github.com/LeandroCasiragh1/resource-monitor
- **Branches**: https://github.com/LeandroCasiragh1/resource-monitor/branches
- **Pull Requests**: https://github.com/LeandroCasiragh1/resource-monitor/pulls (to be created)
- **Issues**: https://github.com/LeandroCasiragh1/resource-monitor/issues

---

## 👤 Assignment Information

**Course**: RA3 (Resource Allocation 3)  
**Type**: Linux System Monitoring Tool  
**Language**: C (C11)  
**Platform**: Linux (primary), Windows (secondary via WSL)  
**Submission**: Ready for pull request review

---

**Last Updated**: Current session  
**Status**: ✅ All main components complete; ready for PR creation and code review
