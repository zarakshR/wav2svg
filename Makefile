P=wav2svg

INCLUDE_DIR=include
SOURCE_DIR=src
OBJECT_DIR=obj
BUILD_DIR=build

CC=gcc
CFLAGS=-Wall -Wextra --std=c18 -I$(INCLUDE_DIR)
LDLIBS=`pkg-config --libs cairo`

_DEPS=util.h draw.h parser.h
DEPS=$(patsubst %, $(INCLUDE_DIR)/%.gch, $(_DEPS))

_OBJS=wav.o draw.o parser.o
OBJS=$(patsubst %,$(OBJECT_DIR)/%, $(_OBJS))

all: default debug
.SECONDARY: $(INCLUDE_DIR)/%.gch
$(INCLUDE_DIR)/%.gch: $(INCLUDE_DIR)/%
	$(CC) $< $(CFLAGS)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

default: $(OBJS)
	$(CC) $(CFLAGS) -O3 -o $(BUILD_DIR)/$(P) $^ $(LDLIBS)

debug: $(OBJS)
	$(CC) $(CFLAGS) -fsanitize=address,undefined,integer-divide-by-zero -fno-omit-frame-pointer -g -O0 -o $(BUILD_DIR)/$(P)_debug $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(INCLUDE_DIR)/*.gch
