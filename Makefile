CC=cc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR) -O2
HEADER_DIR=src/headers

calculator: src/main.c build/eval.o build/intrinsic.o build/operator.o build/parsing.o build/lexing.o build/vartable.o build/tokenstack.o
	$(CC) $(CC_PARAMS) -o calculator src/main.c build/eval.o build/intrinsic.o build/operator.o build/parsing.o build/lexing.o build/vartable.o build/tokenstack.o -lm

build/tokenstack.o: src/tokenstack.c
	$(CC) $(CC_PARAMS) -o build/tokenstack.o src/tokenstack.c -c

build/vartable.o: src/vartable.c
	$(CC) $(CC_PARAMS) -o build/vartable.o src/vartable.c -c

build/lexing.o: src/lexing.c
	$(CC) $(CC_PARAMS) -o build/lexing.o src/lexing.c -c

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o build/parsing.o src/parsing.c -c

build/operator.o: src/operator.c
	$(CC) $(CC_PARAMS) -o build/operator.o src/operator.c -c

build/intrinsic.o: src/intrinsic.c
	$(CC) $(CC_PARAMS) -o build/intrinsic.o src/intrinsic.c -c

build/eval.o: src/eval.c
	$(CC) $(CC_PARAMS) -o build/eval.o src/eval.c -c

clean:
	rm build/*
