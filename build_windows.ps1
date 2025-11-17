# Build script for Windows
# Compila os executáveis Windows do resource-monitor

Write-Host "================================" -ForegroundColor Cyan
Write-Host "Resource Monitor - Windows Build" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan
Write-Host ""

# Criar diretórios
Write-Host "Criando diretórios..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path "bin" -Force > $null
New-Item -ItemType Directory -Path "obj" -Force > $null
New-Item -ItemType Directory -Path "output" -Force > $null

# Compilar resource_profiler_windows
Write-Host "Compilando resource_profiler_windows.c..." -ForegroundColor Yellow
gcc -Wall -Wextra -O2 -Iinclude -std=c11 `
    -c src/resource_profiler_windows.c `
    -o obj/resource_profiler_windows.o

if ($LASTEXITCODE -ne 0) {
    Write-Host "Erro na compilação!" -ForegroundColor Red
    exit 1
}

# Compilar resource_profiler_main
Write-Host "Compilando resource_profiler_main.c..." -ForegroundColor Yellow
gcc -Wall -Wextra -O2 -Iinclude -std=c11 `
    -c src/resource_profiler_main.c `
    -o obj/resource_profiler_main.o

# Linkar executável
Write-Host "Linkando resource-profiler.exe..." -ForegroundColor Yellow
gcc -o bin/resource-profiler.exe `
    obj/resource_profiler_windows.o obj/resource_profiler_main.o `
    -lpsapi -lkernel32

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ resource-profiler.exe compilado com sucesso!" -ForegroundColor Green
    ls -la bin/resource-profiler.exe
} else {
    Write-Host "✗ Erro ao compilar!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Binários disponíveis:" -ForegroundColor Green
ls bin/
Write-Host ""
Write-Host "Para rodar:" -ForegroundColor Cyan
Write-Host "  .\bin\resource-profiler.exe 1 500 3 output.csv" -ForegroundColor White
