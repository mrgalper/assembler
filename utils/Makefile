CC = gcc
CFLAGS = -Wall -Werror -pedantic -ansi 
LDFLAGS = -shared

# Directories
INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target file
TARGET = libutils.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rule to build a binary -- later add it to be dynamic 
binary: $(SRCS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o binary $(filter-out test/logger.c,$(SRCS))

# Rule to run the tests
test: binary
	./binary



