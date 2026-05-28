CC = gcc
SRC = $(wildcard *.c)
LIBSRC = $(wildcard ../../libserialport/*.c)
OBJ = $(SRC:.c=.o) $(LIBSRC:.c=.o)
CFLAGS = -DLIBSERIALPORT_MSBUILD -Wall -I../../libserialport
LDFLAGS = -lsetupapi
DEST = ../../../win
BIN = $(SRC:.c=)

.PHONY: all clean

all: $(BIN) install

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

install: $(BIN)
	@echo "$@ $^..."
	@mv $(BIN) $(DEST)/

clean:
	rm -f $(OBJ) $(DEST)/$(BIN)

