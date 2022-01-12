CC=gcc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR)
HEADER_DIR=src/headers

calculator: src/main.c build/lexing.o build/tokenstack.o
	$(CC) $(CC_PARAMS) -o calculator src/main.c build/lexing.o build/tokenstack.o -lm -O2

build/lexing.o: src/lexing.c
	$(CC) $(CC_PARAMS) -o build/lexing.o src/lexing.c -c -O2

build/tokenstack.o: src/tokenstack.c
	$(CC) $(CC_PARAMS) -o build/tokenstack.o src/tokenstack.c -c -O2

clean:
	rm build/*
