CC=gcc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR)
HEADER_DIR=src/headers

calculator: build/tokenstack.o build/lexing.o build/parsing.o build/intrinsic.o build/eval.o src/main.c
	$(CC) $(CC_PARAMS) -o calculator build/tokenstack.o build/lexing.o build/parsing.o build/intrinsic.o build/eval.o src/main.c -lm -O2

build/tokenstack.o: src/tokenstack.c
	$(CC) $(CC_PARAMS) -o build/tokenstack.o src/tokenstack.c -c -O2

build/lexing.o: src/lexing.c
	$(CC) $(CC_PARAMS) -o build/lexing.o src/lexing.c -c -O2

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o build/parsing.o src/parsing.c -c -O2

build/intrinsic.o: src/intrinsic.c
	$(CC) $(CC_PARAMS) -o build/intrinsic.o src/intrinsic.c -c -O2

build/eval.o: src/eval.c
	$(CC) $(CC_PARAMS) -o build/eval.o src/eval.c -c -O2

clean:
	rm build/*
