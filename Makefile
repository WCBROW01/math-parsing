CC=gcc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR)
HEADER_DIR=src/headers

calculator: src/main.c build/eval.o build/parsing.o build/tokenstack.o build/intrinsic.o
	$(CC) $(CC_PARAMS) -o calculator src/main.c build/eval.o build/parsing.o build/tokenstack.o build/intrinsic.o -lm -O2

build/eval.o: src/eval.c
	$(CC) $(CC_PARAMS) -o build/eval.o src/eval.c -c -O2

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o build/parsing.o src/parsing.c -c -O2

build/tokenstack.o: src/tokenstack.c
	$(CC) $(CC_PARAMS) -o build/tokenstack.o src/tokenstack.c -c -O2

build/intrinsic.o: src/intrinsic.c
	$(CC) $(CC_PARAMS) -o build/intrinsic.o src/intrinsic.c -c -O2

debug: src/main.c src/eval.c src/parsing.c src/tokenstack.c src/intrinsic.c
	$(CC) $(CC_PARAMS) -o calculator src/main.c src/eval.c src/parsing.c src/tokenstack.c src/intrinsic.c -lm -g

clean:
	rm build/*
