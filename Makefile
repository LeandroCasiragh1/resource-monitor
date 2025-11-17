CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -std=gnu11
LDFLAGS =
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

BIN_DIR = bin

# Separate binaries for each tool
RESOURCE_PROFILER_BIN = $(BIN_DIR)/resource-profiler
NAMESPACE_ANALYZER_BIN = $(BIN_DIR)/namespace-analyzer
CGROUP_MANAGER_BIN = $(BIN_DIR)/cgroup-manager
TEST_RUNNER_BIN = $(BIN_DIR)/test_runner

# Default: build all tools
all: $(RESOURCE_PROFILER_BIN) $(NAMESPACE_ANALYZER_BIN) $(CGROUP_MANAGER_BIN)

# Resource Profiler
$(RESOURCE_PROFILER_BIN): src/resource_profiler.o src/resource_profiler_main.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Namespace Analyzer
$(NAMESPACE_ANALYZER_BIN): src/namespace_analyzer.o src/namespace_analyzer_main.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Cgroup Manager
$(CGROUP_MANAGER_BIN): src/cgroup_manager.o src/cgroup_manager_main.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Object file compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Tests target
tests: CFLAGS += -DUNIT_TEST
tests: $(TEST_RUNNER_BIN)

$(TEST_RUNNER_BIN): $(filter-out src/main.o src/resource_profiler_main.o src/namespace_analyzer_main.o src/cgroup_manager_main.o,$(OBJS)) $(wildcard tests/*.c) | $(BIN_DIR)
	$(CC) $(CFLAGS) -Iinclude -o $@ $(filter-out src/main.o src/resource_profiler_main.o src/namespace_analyzer_main.o src/cgroup_manager_main.o,$(OBJS)) tests/*.c

# Install rule (optional: copy binaries to /usr/local/bin)
install: all
	@echo "Install target: copy binaries to /usr/local/bin"
	@for bin in $(RESOURCE_PROFILER_BIN) $(NAMESPACE_ANALYZER_BIN) $(CGROUP_MANAGER_BIN); do \
		echo "  Installing $$bin to /usr/local/bin/"; \
	done

# Check: compile without linking (syntax check)
check:
	$(CC) $(CFLAGS) -c src/resource_profiler.c
	$(CC) $(CFLAGS) -c src/namespace_analyzer.c
	$(CC) $(CFLAGS) -c src/cgroup_manager.c
	@echo "Syntax check passed"

# Clean rule
clean:
	rm -rf $(BIN_DIR) src/*.o tests/*.o

.PHONY: all tests install check clean
