MAIN = main
TARGET = bin/$(MAIN)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
INCLUDE_SRCS = $(wildcard $(INCLUDE_DIR)/*.c)

# Object files: mirror directory structure inside obj/
OBJ_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS) $(INCLUDE_SRCS))


# Default target
all: $(TARGET)

# Create directories if they don't exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Build object files from src
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


# Build object files from include
$(OBJ_DIR)/%.o: $(INCLUDE_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

# Run program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*
	rm -f compile_commands.json

# Format
format:
	clang-format -i $(SRCS) $(INCLUDE_SRCS)

# Lint
lint:
	@if [ ! -f compile_commands.json ]; then \
		echo "Generating compile_commands.json with bear..."; \
		bear -- make all; \
	fi
	clang-tidy $(SRCS) $(INCLUDE_SRCS)

# Check: format + lint
check: format lint

# Debug build
debug: CFLAGS += -g
debug: $(TARGET)

# Profiling build
profile: CFLAGS += -pg
profile: $(TARGET)

