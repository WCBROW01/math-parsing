CC=cc
CFLAGS=-Wall -Wextra -I$(HEADER_DIR) -O2
HEADER_DIR=src/headers

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

calculator: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -f $(OBJ)
