# PowerShell build script for resource-monitor (Windows port)
# Usage: .\build.ps1

$CC = "gcc"
$CFLAGS = "-Wall -Wextra -O2 -Iinclude -std=c11"
$BIN_DIR = "bin"
$SRC_DIR = "src"

# Create bin directory if it doesn't exist
if (-not (Test-Path $BIN_DIR)) {
    New-Item -ItemType Directory -Path $BIN_DIR | Out-Null
    Write-Host "Created $BIN_DIR directory" -ForegroundColor Green
}

Write-Host "Building resource-monitor (Windows port)..." -ForegroundColor Cyan

# Compile resource-profiler (Windows version using Windows APIs)
Write-Host "`nCompiling resource-profiler (Windows)..." -ForegroundColor Yellow
& $CC $CFLAGS.Split() -c "$SRC_DIR/resource_profiler_windows.c" -o "$BIN_DIR/resource_profiler.o" 2>&1
if ($LASTEXITCODE -eq 0) {
    & $CC $CFLAGS.Split() -c "$SRC_DIR/resource_profiler_main.c" -o "$BIN_DIR/resource_profiler_main.o" 2>&1
    & $CC -o "$BIN_DIR/resource-profiler.exe" "$BIN_DIR/resource_profiler.o" "$BIN_DIR/resource_profiler_main.o" -lpsapi -lkernel32 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ resource-profiler built successfully" -ForegroundColor Green
    } else {
        Write-Host "✗ Failed to link resource-profiler" -ForegroundColor Red
    }
} else {
    Write-Host "✗ Failed to compile resource-profiler" -ForegroundColor Red
}

# Compile namespace-analyzer (Windows stub - Linux-only feature)
Write-Host "`nCompiling namespace-analyzer (Windows stub)..." -ForegroundColor Yellow
& $CC $CFLAGS.Split() -c "$SRC_DIR/namespace_analyzer_windows.c" -o "$BIN_DIR/namespace_analyzer.o" 2>&1
if ($LASTEXITCODE -eq 0) {
    & $CC $CFLAGS.Split() -c "$SRC_DIR/namespace_analyzer_main.c" -o "$BIN_DIR/namespace_analyzer_main.o" 2>&1
    & $CC -o "$BIN_DIR/namespace-analyzer.exe" "$BIN_DIR/namespace_analyzer.o" "$BIN_DIR/namespace_analyzer_main.o" 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ namespace-analyzer (stub) built successfully" -ForegroundColor Yellow
    } else {
        Write-Host "✗ Failed to link namespace-analyzer" -ForegroundColor Red
    }
} else {
    Write-Host "✗ Failed to compile namespace-analyzer" -ForegroundColor Red
}

# Compile cgroup-manager (Windows stub - Linux-only feature)
Write-Host "`nCompiling cgroup-manager (Windows stub)..." -ForegroundColor Yellow
& $CC $CFLAGS.Split() -c "$SRC_DIR/cgroup_manager_windows.c" -o "$BIN_DIR/cgroup_manager.o" 2>&1
if ($LASTEXITCODE -eq 0) {
    & $CC $CFLAGS.Split() -c "$SRC_DIR/cgroup_manager_main.c" -o "$BIN_DIR/cgroup_manager_main.o" 2>&1
    & $CC -o "$BIN_DIR/cgroup-manager.exe" "$BIN_DIR/cgroup_manager.o" "$BIN_DIR/cgroup_manager_main.o" 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ cgroup-manager (stub) built successfully" -ForegroundColor Yellow
    } else {
        Write-Host "✗ Failed to link cgroup-manager" -ForegroundColor Red
    }
} else {
    Write-Host "✗ Failed to compile cgroup-manager" -ForegroundColor Red
}

Write-Host "`n✓ Build completed!" -ForegroundColor Green
Write-Host "`nBinaries created:" -ForegroundColor Cyan
Get-ChildItem "$BIN_DIR/*.exe" 2>$null | ForEach-Object { Write-Host "  - $($_.Name)" }

Write-Host "`nTo run the tools:" -ForegroundColor Cyan
Write-Host "  .\bin\resource-profiler.exe <PID> [interval_ms] [samples] [out.csv]" -ForegroundColor Gray
Write-Host "`nNote: namespace-analyzer and cgroup-manager require Linux (use WSL2 or native Linux VM)" -ForegroundColor DarkYellow

Write-Host "`nTo run the tools:" -ForegroundColor Cyan
Write-Host "  .\bin\resource-profiler.exe <PID> [interval_ms] [samples] [out.csv]" -ForegroundColor Gray
Write-Host "  .\bin\namespace-analyzer.exe list <PID>" -ForegroundColor Gray
Write-Host "  .\bin\cgroup-manager.exe read <cgroup_path>" -ForegroundColor Gray
Write-Host "  .\bin\test_runner.exe" -ForegroundColor Gray
