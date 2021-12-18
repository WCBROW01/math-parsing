#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "opstack.h"

OpStack OpStack_new() {
	OpStack stack = {
		.length = 0,
		.top = -1,
		.ops = malloc(MAX_LENGTH * sizeof(Op))
	};

	if (stack.ops == NULL) {
		fprintf(stderr, "Failed to create OpStack, exiting.\n");
		exit(1);
	}

	return stack;
}

void OpStack_delete(OpStack *stack) {
	free(stack->ops);
}

void OpStack_push(OpStack *stack, const Op *data) {
	assert(++stack->length <= 64 && "Error: You have too many items in the stack.");
	stack->ops[++stack->top] = *data;
}

Op OpStack_pop(OpStack *stack) {
	assert(--stack->length >= 0 && "Error: You have popped more items than are in the stack.");
	return stack->ops[stack->top--];
}

Op OpStack_peek(const OpStack *stack) {
	return stack->ops[stack->top];
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

void OpStack_print(const OpStack *stack) {
	for (int i = 0; i <= stack->top; i++) {
		if (stack->ops[i % MAX_LENGTH].isOperator) {
			printf("%c ", operatorToChar(&stack->ops[i]));
		} else {
			printf("%ld ", stack->ops[i].data);
		}
	}

	printf("\n");
}
