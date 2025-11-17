#################################################################################
# Makefile - Resource Monitor
# Build system com suporte a múltiplas plataformas
#################################################################################

CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -std=c11
CFLAGS_NCURSES = $(shell pkg-config --cflags ncurses 2>/dev/null || echo "-I/usr/include")
LDFLAGS = $(shell pkg-config --libs ncurses 2>/dev/null || echo "-lncurses")

# Diretórios
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
OUTPUT_DIR = output

# Fonte e objetos
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Binários
MONITOR_BIN = $(BIN_DIR)/monitor
CGROUP_MGR_BIN = $(BIN_DIR)/cgroup_manager
TEST_RUNNER_BIN = $(BIN_DIR)/test_runner

# Default: compilar tudo
.PHONY: all
all: $(MONITOR_BIN) $(CGROUP_MGR_BIN)
	@echo ""
	@echo "✓ Build completo!"
	@echo "  Binários gerados:"
	@ls -lh $(MONITOR_BIN) $(CGROUP_MGR_BIN) 2>/dev/null | awk '{print "    " $$9 " (" $$5 ")"}'

# Monitor principal
$(MONITOR_BIN): $(OBJ_DIR)/monitor_tui.o $(OBJ_DIR)/resource_profiler.o \
                $(OBJ_DIR)/namespace_analyzer.o $(OBJ_DIR)/cpu_monitor.o \
                $(OBJ_DIR)/memory_monitor.o $(OBJ_DIR)/io_monitor.o \
                $(OBJ_DIR)/network_monitor.o $(OBJ_DIR)/utils.o \
                $(OBJ_DIR)/cgroup_manager.o $(OBJ_DIR)/cgroup_v2.o $(OBJ_DIR)/process_monitor.o \
                $(OBJ_DIR)/experiments.o $(OBJ_DIR)/experiment_overhead.o \
                $(OBJ_DIR)/experiment_cpu_throttling.o $(OBJ_DIR)/experiment_memory_limit.o \
                $(OBJ_DIR)/experiment_io_limit.o | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) $(CFLAGS_NCURSES) -o $@ $^ $(LDFLAGS)
	@echo "✓ $@ compilado"

# Gerenciador de Cgroups (usa implementação própria em cgroup_manager.c + main)
$(CGROUP_MGR_BIN): $(OBJ_DIR)/cgroup_manager.o $(OBJ_DIR)/cgroup_manager_main.o \
                   $(OBJ_DIR)/utils.o | $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ $@ compilado"

# Diretórios
$(BIN_DIR) $(OBJ_DIR) $(OUTPUT_DIR):
	@mkdir -p $@

# Compilação de objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(OUTPUT_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(CFLAGS_NCURSES) -I$(INCLUDE_DIR) -c $< -o $@

# Tests
.PHONY: tests
tests: $(TEST_RUNNER_BIN)
	@echo "Rodando testes..."
	@./$(TEST_RUNNER_BIN)

$(TEST_RUNNER_BIN): $(filter-out $(OBJ_DIR)/monitor_tui.o $(OBJ_DIR)/cgroup_manager.o, $(OBJS)) \
                    $(wildcard $(TEST_DIR)/*.o) | $(BIN_DIR)
	@echo "Linking tests..."
	@$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ \
           $(filter-out $(OBJ_DIR)/monitor_tui.o $(OBJ_DIR)/cgroup_manager.o, $(OBJS)) \
           $(TEST_DIR)/*.c 2>/dev/null || true

# Limpeza
.PHONY: clean
clean:
	@echo "Limpando..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(OUTPUT_DIR)/*.csv $(OUTPUT_DIR)/*.json
	@echo "✓ Limpeza completa"

# Limpar tudo
.PHONY: distclean
distclean: clean
	@rm -rf $(OUTPUT_DIR)
	@echo "✓ Todos os arquivos gerados removidos"

# Instalação
.PHONY: install
install: all
	@echo "Instalando binários..."
	@install -d /usr/local/bin
	@install -m 755 $(MONITOR_BIN) /usr/local/bin/resource-monitor
	@install -m 755 $(CGROUP_MGR_BIN) /usr/local/bin/cgroup-manager
	@echo "✓ Instalado em /usr/local/bin"

# Help
.PHONY: help
help:
	@echo "Resource Monitor - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Compila tudo"
	@echo "  make tests        - Roda testes"
	@echo "  make install      - Instala binários"
	@echo "  make clean        - Remove arquivos compilados"
	@echo "  make distclean    - Remove tudo exceto fonte"
	@echo "  make help         - Mostra esta mensagem"
