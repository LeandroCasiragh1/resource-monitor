CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -std=gnu11
LDFLAGS =
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

BIN_DIR = bin
MAIN_BIN = $(BIN_DIR)/resource-monitor
PROFILER_BIN = $(BIN_DIR)/resource-profiler

all: $(MAIN_BIN) $(PROFILER_BIN)

$(MAIN_BIN): src/main.o $(filter-out src/resource_profiler.o src/resource_profiler_main.o,$(OBJS)) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(PROFILER_BIN): src/resource_profiler.o src/resource_profiler_main.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: CFLAGS += -DUNIT_TEST

tests: CFLAGS += -DUNIT_TEST
tests: $(BIN_DIR)/test_runner

$(BIN_DIR)/test_runner: $(filter-out src/main.o,$(OBJS)) $(wildcard tests/*.c) | $(BIN_DIR)
	$(CC) $(CFLAGS) -Iinclude -o $@ $(filter-out src/main.o,$(OBJS)) tests/*.c

clean:
	rm -rf $(BIN_DIR)/*.o src/*.o $(BIN_DIR)/*

.PHONY: all clean tests
