CC = gcc
CFLAGS = -pedantic -ansi -Wall -std=c90
SRC_DIR = src
UTILS_DIR = utils/src
INCLUDE_DIR = include
UTILS_INCLUDE_DIR = utils/include
EXECUTABLE = assembler

# List of source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
UTILS_SRC_FILES = $(wildcard $(UTILS_DIR)/*.c)

# List of header files
INCLUDE_FILES = $(wildcard $(INCLUDE_DIR)/*.h)
UTILS_INCLUDE_FILES = $(wildcard $(UTILS_INCLUDE_DIR)/*.h)

# Object files
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))
UTILS_OBJ_FILES = $(patsubst %.c, %.o, $(UTILS_SRC_FILES))

# Include directories
INCLUDE_FLAGS = -I$(INCLUDE_DIR) -I$(UTILS_INCLUDE_DIR)

$(EXECUTABLE): $(OBJ_FILES) $(UTILS_OBJ_FILES) assembler.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(INCLUDE_FILES) $(UTILS_INCLUDE_FILES)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

assembler.o: assembler.c $(INCLUDE_FILES) $(UTILS_INCLUDE_FILES)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJ_FILES) $(UTILS_OBJ_FILES) assembler.o

.PHONY: clean
