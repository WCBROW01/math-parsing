CC=gcc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR)
HEADER_DIR=src/headers

calculator: src/main.c build/parsing.o build/opstack.o
	$(CC) $(CC_PARAMS) -o calculator src/main.c build/parsing.o build/opstack.o -O2

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o build/parsing.o src/parsing.c -c -O2

build/opstack.o: src/opstack.c
	$(CC) $(CC_PARAMS) -o build/opstack.o src/opstack.c -c -O2

debug: src/main.c src/parsing.c src/opstack.c
	$(CC) $(CC_PARAMS) -o calculator src/main.c src/parsing.c src/opstack.c -g

clean:
	rm build/*
