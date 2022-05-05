BIN=gcc
CFLAGS=-g -Wall -Werror -Wextra

all: neko

neko: 
	$(BIN) $(CFLAGS) neko.c -o $@

clean:

ifeq ($(OS),Windows_NT)
	del neko.exe
else
	rm neko
endif