CC=cc
CFLAGS=-Wall -Wextra -I$(HEADER_DIR) -O2
HEADER_DIR=src/headers

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
LDFLAGS=-lm

calculator: $(OBJ)

clean:
	rm -f $(OBJ)
