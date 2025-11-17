CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = bin/resource-monitor

all: $(BIN)

$(BIN): $(OBJ) | bin
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

bin:
	mkdir -p bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests: CFLAGS += -DUNIT_TEST

tests: CFLAGS += -DUNIT_TEST
tests: $(BIN_DIR)/test_runner

$(BIN_DIR)/test_runner: $(filter-out src/main.o,$(OBJS)) $(wildcard tests/*.c) | $(BIN_DIR)
	$(CC) $(CFLAGS) -Iinclude -o $@ $(filter-out src/main.o,$(OBJS)) tests/*.c

clean:
	rm -rf bin/*.o src/*.o bin resource-monitor

.PHONY: all clean tests
