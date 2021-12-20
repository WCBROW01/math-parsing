#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "opstack.h"

TokenStack TokenStack_new() {
	TokenStack stack = {
		.length = 0,
		.top = -1,
		.ops = malloc(MAX_LENGTH * sizeof(Token))
	};

	if (stack.ops == NULL) {
		fprintf(stderr, "Failed to create TokenStack, exiting.\n");
		exit(1);
	}

	return stack;
}

void TokenStack_delete(TokenStack *stack) {
	free(stack->ops);
}

void TokenStack_push(TokenStack *stack, const Token *data) {
	assert(++stack->length <= 64 && "Error: You have too many items in the stack.");
	stack->ops[++stack->top] = *data;
}

Token TokenStack_pop(TokenStack *stack) {
	assert(--stack->length >= 0 && "Error: You have popped more items than are in the stack.");
	return stack->ops[stack->top--];
}

Token TokenStack_peek(const TokenStack *stack) {
	return stack->ops[stack->top];
}

char operatorToChar(const Token *operator) {
	switch((int) operator->data) {
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
		fprintf(stderr, "Invalid operator '%.15Lg'.\n", operator->data);
		exit(2);
	}
}

void TokenStack_print(const TokenStack *stack) {
	for (int i = 0; i <= stack->top; i++) {
		if (stack->ops[i].isOperator) {
			printf("%c ", operatorToChar(&stack->ops[i]));
		} else {
			printf("%.15Lg ", stack->ops[i].data);
		}
	}

	printf("\n");
}
