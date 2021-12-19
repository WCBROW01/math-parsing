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
} Op;

typedef struct {
	int length;
	int top;
	Op *ops;
} OpStack;

extern OpStack OpStack_new();
extern void OpStack_delete(OpStack *stack);

extern void OpStack_push(OpStack *stack, const Op *data);
extern Op OpStack_pop(OpStack *stack);
extern Op OpStack_peek(const OpStack *stack);

extern char operatorToChar(const Op *operator);
extern void OpStack_print(const OpStack *stack);

#endif
