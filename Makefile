# Compiler and flags
CC = gcc

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

CFLAGS = -Wall -g -std=c99 -I$(INCLUDE_DIR)
LDFLAGS = -lm

# Source files
COMMON_SOURCES = $(wildcard $(SRC_DIR)/chess_logic.c $(SRC_DIR)/legal_moves.c)
GAME_SOURCES = $(wildcard $(SRC_DIR)/chess.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/chess_tests.c)

# Object files
COMMON_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(COMMON_SOURCES))
GAME_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(GAME_SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

# Test executable
TEST_TARGET = $(BIN_DIR)/chess_tests

# Interactive game executable
GAME_TARGET = $(BIN_DIR)/chess

# The default target to build both
.PHONY: all clean test game
all: game test

test: $(TEST_TARGET)

game: $(GAME_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS) $(COMMON_OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(GAME_TARGET): $(GAME_OBJECTS) $(COMMON_OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile source files from src/ and tests/ into build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create output directories if they don't exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
