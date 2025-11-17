#!/bin/bash

#################################################################################
# Build Script - Resource Monitor
# Este script compila todo o código fonte e executa o menu interativo
#################################################################################

set -e

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para imprimir com cor
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_section() {
    echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}$1${NC}"
    echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
}

# Diretórios
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="$PROJECT_ROOT/bin"
OBJ_DIR="$PROJECT_ROOT/obj"
OUTPUT_DIR="$PROJECT_ROOT/output"

print_section "Resource Monitor - Build System"

# Criar diretórios necessários
print_info "Criando estrutura de diretórios..."
mkdir -p "$BIN_DIR"
mkdir -p "$OBJ_DIR"
mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/graphs"
mkdir -p "$OUTPUT_DIR/experiments"
print_success "Diretórios criados"

# Verificar dependências
print_info "Verificando dependências..."

check_command() {
    if ! command -v "$1" &> /dev/null; then
        print_error "$1 não encontrado"
        return 1
    fi
    print_success "$1 encontrado"
    return 0
}

check_command "gcc" || exit 1
check_command "make" || exit 1

# Tentar encontrar ncurses (desenvolvimento)
if ! pkg-config --exists ncurses 2>/dev/null; then
    print_error "ncurses development files não encontrados"
    echo -e "${YELLOW}Para instalar:${NC}"
    echo -e "  ${BLUE}Ubuntu/Debian:${NC} sudo apt-get install libncurses-dev"
    echo -e "  ${BLUE}Fedora/RHEL:${NC}   sudo dnf install ncurses-devel"
    echo -e "  ${BLUE}Arch:${NC}         sudo pacman -S ncurses"
    echo ""
    read -p "Continuar mesmo sem ncurses? (s/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Ss]$ ]]; then
        exit 1
    fi
else
    print_success "ncurses encontrado"
fi

# Compilação
print_section "Compilando código-fonte"

if make -C "$PROJECT_ROOT" clean > /dev/null 2>&1; then
    print_success "Limpeza anterior concluída"
fi

if make -C "$PROJECT_ROOT"; then
    print_success "Compilação concluída com sucesso"
else
    print_error "Compilação falhou"
    exit 1
fi

# Verificar binários gerados
print_section "Verificando binários gerados"

if [ -f "$BIN_DIR/monitor" ]; then
    SIZE=$(du -h "$BIN_DIR/monitor" | cut -f1)
    print_success "monitor ($SIZE) compilado"
else
    print_error "Binário monitor não encontrado"
fi

if [ -f "$BIN_DIR/cgroup_manager" ]; then
    SIZE=$(du -h "$BIN_DIR/cgroup_manager" | cut -f1)
    print_success "cgroup_manager ($SIZE) compilado"
else
    print_info "cgroup_manager não compilado (opcional)"
fi

# Listar todos os binários
echo ""
print_info "Binários disponíveis:"
ls -lh "$BIN_DIR" | grep -v "^total" | awk '{print "  " $9 " (" $5 ")"}'

# Menu de opções após compilação
print_section "Compilação Concluída"

echo -e "${BLUE}Escolha uma opção:${NC}"
echo "  1) Executar menu interativo (./bin/monitor menu)"
echo "  2) Executar TUI direto"
echo "  3) Ver ajuda (./bin/monitor --help)"
echo "  4) Apenas compilar (sair)"
echo ""

read -p "Opção [1-4]: " -n 1 -r option
echo

case $option in
    1)
        print_info "Iniciando menu interativo..."
        sleep 1
        if [ ! -t 0 ]; then
            # Se estamos em pipe, usar bash
            exec bash -c "cd '$PROJECT_ROOT' && '$BIN_DIR/monitor' menu"
        else
            "$BIN_DIR/monitor" menu
        fi
        ;;
    2)
        print_info "Iniciando TUI..."
        sleep 1
        "$BIN_DIR/monitor" tui 1 5 60
        ;;
    3)
        "$BIN_DIR/monitor" --help
        ;;
    4)
        print_success "Build completo. Execute './bin/monitor' para começar"
        ;;
    *)
        echo "Opção inválida"
        ;;
esac

print_success "Encerrado com sucesso"
