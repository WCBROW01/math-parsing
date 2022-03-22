CC=cc
CC_PARAMS=-Wall -Wextra -pedantic -I$(HEADER_DIR) -g
HEADER_DIR=src/headers

calculator: src/main.c build/eval.o build/intrinsic.o build/operator.o build/parsing.o build/lexing.o build/vartable.o build/arena.o build/tokenstack.o
	$(CC) $(CC_PARAMS) -o $@ $^ -lm

build/tokenstack.o: src/tokenstack.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/vartable.o: src/vartable.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/lexing.o: src/lexing.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/parsing.o: src/parsing.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/operator.o: src/operator.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/intrinsic.o: src/intrinsic.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/eval.o: src/eval.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

build/arena.o: src/arena.c
	$(CC) $(CC_PARAMS) -o $@ $^ -c

clean:
	rm build/*
