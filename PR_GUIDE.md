# PR Creation Guide

## Current Status

Already merged PRs (visible in git log):
- ✅ #1 resource-profiler → master
- ✅ #2 cgroup-manager → master
- ✅ #3 namespace-analyzer → master
- ✅ #4 docs → master
- ✅ #5 tests → master
- ✅ #6 makefile-update → master
- ✅ #7 profiler-cpu-percent → master

## Remaining Feature Branches

### New Branches to Create PRs For:

1. **windows-port** (CURRENT)
   - Status: Ready for PR (3 commits on windows-port)
   - Changes: Windows implementations + build script + documentation
   - Files: resource_profiler_windows.c, build.ps1, WINDOWS_PORT.md
   - Type: Platform support (optional/can be separate PR or squashed)

### How to Create PRs via GitHub Web UI

1. **Visit**: https://github.com/LeandroCasiragh1/resource-monitor
2. **Click**: "Pull requests" tab
3. **Click**: "New pull request" button
4. **Select branches**:
   - Base: `master`
   - Compare: `windows-port` (or any feature branch)
5. **Fill in**:
   - Title: "[FEATURE] Windows port with resource profiler support"
   - Description: (see template below)
6. **Click**: "Create pull request"

## PR Template for windows-port

```markdown
## Windows Port Implementation

### Description
Adds Windows-compatible versions of resource monitoring tools using Windows API.

### Changes
- `resource_profiler_windows.c`: Uses Windows API (GetProcessTimes, GetProcessMemoryInfo)
- `namespace_analyzer_windows.c`: Linux-only stub with informative error
- `cgroup_manager_windows.c`: Linux-only stub with informative error
- `build.ps1`: Updated PowerShell build script for Windows compilation
- `docs/WINDOWS_PORT.md`: Comprehensive Windows port guide

### Features
✅ Resource Profiler functional on Windows (user/kernel time, memory metrics)
⚠️ Namespace Analyzer / Cgroup Manager: Informative stubs (requires Linux)
✅ WSL2 support for full functionality

### Type of Change
- [x] New feature (non-breaking change which adds functionality)
- [x] Platform/Portability enhancement
- [ ] Breaking change

### Testing
- [x] Build script tested on Windows (GCC/MinGW)
- [x] Compiles without errors
- [ ] Need manual testing on target platform

### Documentation
- [x] Updated WINDOWS_PORT.md with comprehensive guide
- [x] Build instructions included
- [x] Usage examples provided

### Notes
- Namespace and Cgroup features are Linux-kernel-specific and cannot be ported to Windows
- Recommended for Windows developers: Use WSL2 for full feature support
- Resource Profiler provides equivalent functionality using Windows APIs

### Checklist
- [x] Code follows project style guidelines
- [x] Self-review of code completed
- [x] Comments added for complex sections
- [x] Documentation updated
- [x] No new warnings generated
- [x] Tested on Windows environment
```

## Alternative: Using GitHub CLI

If you have `gh` CLI installed:

```powershell
# Create PR for windows-port
gh pr create `
  --title "feat: Add Windows port with resource profiler support" `
  --body "Windows-compatible implementations using Windows API. Namespace/cgroup Linux-only stubs." `
  --base master `
  --head windows-port
```

## Merging Strategy

### After PR Creation:
1. Review own code changes
2. Check for conflicts (should be none)
3. Merge with "Squash and merge" or "Create a merge commit"
4. Delete branch after merge (optional)

### Recommended Merge Order (if any dependencies):
1. windows-port (independent; can be merged anytime)
2. Other branches (already merged)

## Branch Cleanup

After merging PR, optionally delete the feature branch:

```bash
# Delete local branch
git branch -d windows-port

# Delete remote branch
git push origin --delete windows-port
```

## Post-Merge Tasks

1. Update CHANGELOG.md with Windows port changes
2. Tag a new release if appropriate (v1.0.0-windows or similar)
3. Update main README.md with Windows support info
4. Create GitHub Releases page

---

**Note**: Since many branches are already merged, check GitHub UI to see current PR status.
