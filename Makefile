P=wav2svg
OBJECTS=wav.c parser.c defs.h
CFLAGS=-Wall
CC=gcc
BUILD_DIR=build

default: $(P)
all: $(P) debug

$(P): $(OBJECTS)
	$(CC) $(CFLAGS) -O3 -o $(BUILD_DIR)/$(P) $(OBJECTS)

debug: $(OBJECTS)
	$(CC) $(CFLAGS) -g -O0 -o $(BUILD_DIR)/$(P)_debug $(OBJECTS)
