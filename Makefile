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
tests: test_runner

test_runner: $(wildcard tests/*.c) $(filter src/main.o,$(OBJ))
	$(CC) $(CFLAGS) -Iinclude -o bin/test_runner tests/*.c $(filter-out src/main.o,$(OBJ))

clean:
	rm -rf bin/*.o src/*.o bin resource-monitor

.PHONY: all clean tests
