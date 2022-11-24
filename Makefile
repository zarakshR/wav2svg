P=wav2svg
CFLAGS=`pkg-config --cflags cairo` -Wall -iquote include/
CC=gcc
BUILD_DIR=build
SOURCE_DIR=src
INCLUDE_DIR=include
LDLIBS=`pkg-config --libs cairo`

default: $(P)
all: $(P) debug

$(P): $(SOURCE_DIR)/*.c $(INCLUDE_DIR)/*.h
	$(CC) $(CFLAGS) -O3 -o $(BUILD_DIR)/$(P) $(SOURCE_DIR)/*.c $(INCLUDE_DIR)/*.h $(LDLIBS)

debug: $(SOURCE_DIR)/*.c $(INCLUDE_DIR)/*.h
	$(CC) $(CFLAGS) -g -O0 -o $(BUILD_DIR)/$(P)_debug $(SOURCE_DIR)/*.c $(INCLUDE_DIR)/*.h $(LDLIBS)
