CC=gcc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR)
HEADER_DIR=src/headers

calculator: src/main.c build/parsing.o build/opdeque.o
	$(CC) $(CC_PARAMS) -o calculator src/main.c build/parsing.o build/opdeque.o -O2

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o build/parsing.o src/parsing.c -c -O2

build/opdeque.o: src/opdeque.c
	$(CC) $(CC_PARAMS) -o build/opdeque.o src/opdeque.c -c -O2

debug: src/main.c src/parsing.c src/opdeque.c
	$(CC) $(CC_PARAMS) -o calculator src/main.c src/parsing.c src/opdeque.c -g

clean:
	rm build/*
