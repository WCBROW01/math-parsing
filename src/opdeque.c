#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "opdeque.h"

OpDeque OpDeque_new() {
	OpDeque deque = {
		.length = 0,
		.back = -1,
		.front = 0,
		.ops = malloc(MAX_LENGTH * sizeof(Op))
	};

	if (deque.ops == NULL) {
		fprintf(stderr, "Failed to create OpDeque, exiting.\n");
		exit(1);
	}

	return deque;
}

void OpDeque_delete(OpDeque *deque) {
	free(deque->ops);
}

void OpDeque_push(OpDeque *deque, const Op *data) {
	assert(++deque->length <= 64 && "Error: You have too many items in the deque.");
	deque->ops[++deque->back % MAX_LENGTH] = *data;
}

Op OpDeque_popBack(OpDeque *deque) {
	assert(--deque->length >= 0 && "Error: You have popped more items than are in the deque.");
	return deque->ops[deque->back-- % MAX_LENGTH];
}

Op OpDeque_popFront(OpDeque *deque) {
	assert(--deque->length >= 0 && "Error: You have popped more items than are in the deque.");
	return deque->ops[deque->front++ % MAX_LENGTH];
}

Op OpDeque_peekBack(const OpDeque *deque) {
	return deque->ops[deque->back % MAX_LENGTH];
}

Op OpDeque_peekFront(const OpDeque *deque) {
	return deque->ops[deque->front % MAX_LENGTH];
}

char operatorToChar(const Op *operator) {
	switch(operator->data) {
	case ADD:
		return '+';
	case SUB:
		return '-';
	case MUL:
		return '*';
	case DIV:
		return '/';
	case EXP:
		return '^';
	case OPEN_PAREN:
		return '(';
	case CLOSE_PAREN:
		return ')';
	default:
		fprintf(stderr, "Invalid operator '%ld'.\n", operator->data);
		exit(2);
	}
}

void OpDeque_print(const OpDeque *deque) {
	for (long i = deque->front; i <= deque->back; i++) {
		if (deque->ops[i % MAX_LENGTH].isOperator) {
			printf("%c ", operatorToChar(&deque->ops[i % MAX_LENGTH]));
		} else {
			printf("%ld ", deque->ops[i % MAX_LENGTH].data);
		}
	}

	printf("\n");
}
