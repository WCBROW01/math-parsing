#ifndef OPDEQUE_H
#define OPDEQUE_H

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
	long data;
} Op;

typedef struct {
	int length;
	int back;
	int front;
	Op *ops;
} OpDeque;

extern OpDeque OpDeque_new();
extern void OpDeque_delete(OpDeque *deque);

extern void OpDeque_push(OpDeque *deque, const Op *data);
extern Op OpDeque_popBack(OpDeque *deque);
extern Op OpDeque_popFront(OpDeque *deque);
extern Op OpDeque_peekBack(const OpDeque *deque);
extern Op OpDeque_peekFront(const OpDeque *deque);

extern char operatorToChar(const Op *operator);
extern void OpDeque_print(const OpDeque *deque);

#endif
