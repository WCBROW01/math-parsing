#ifndef OPSTACK_H
#define OPSTACK_H

#include <stdbool.h>

#define MAX_LENGTH 64
#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define EXP 4
#define OPEN_PAREN 5
#define CLOSE_PAREN 6

// Struct for operators and operands (with a boolean to differentiate)
typedef struct {
	bool isOperator;
	long double data;
} Token;

typedef struct {
	int length;
	int top;
	Token *ops;
} TokenStack;

extern TokenStack TokenStack_new();
extern void TokenStack_delete(TokenStack *stack);

extern void TokenStack_push(TokenStack *stack, const Token *data);
extern Token TokenStack_pop(TokenStack *stack);
extern Token TokenStack_peek(const TokenStack *stack);

extern char operatorToChar(const Token *operator);
extern void TokenStack_print(const TokenStack *stack);

#endif
