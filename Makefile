P=wav2svg
OBJECTS=wav.c parser.c helpers.c defs.h
CFLAGS=-Wall
CC=gcc

default: $(P)
all: $(P) debug

$(P): $(OBJECTS)
	$(CC) $(CFLAGS) -O3 -o $(P) $(OBJECTS)

debug: $(OBJECTS)
	$(CC) $(CFLAGS) -g -O0 -o $(P)_debug $(OBJECTS)