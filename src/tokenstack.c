#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokenstack.h"

TokenStack TokenStack_new() {
	TokenStack stack = {
		.length = 0,
		.top = -1,
		.tokens = malloc(MAX_LENGTH * sizeof(Token))
	};

	if (stack.tokens == NULL) {
		fprintf(stderr, "Failed to create TokenStack, exiting.\n");
		exit(1);
	}

	return stack;
}

void TokenStack_delete(TokenStack *stack) {
	free(stack->tokens);
}

void TokenStack_push(TokenStack *stack, const Token *data) {
	assert(++stack->length <= 64 && "Error: You have too many items in the stack.");
	stack->tokens[++stack->top] = *data;
}

Token TokenStack_pop(TokenStack *stack) {
	assert(--stack->length >= 0 && "Error: You have popped more items than are in the stack.");
	return stack->tokens[stack->top--];
}

Token TokenStack_peek(const TokenStack *stack) {
	return stack->tokens[stack->top];
}

char operatorToChar(const Token *operator) {
	switch(operator->op) {
	case Add:
		return '+';
	case Sub:
		return '-';
	case Mul:
		return '*';
	case Div:
		return '/';
	case Exp:
		return '^';
	case OpenParen:
		return '(';
	case CloseParen:
		return ')';
	default:
		fprintf(stderr, "Invalid operator '%d'.\n", operator->op);
		exit(2);
	}
}

void TokenStack_print(const TokenStack *stack) {
	for (int i = 0; i <= stack->top; i++) {
		if (stack->tokens[i].op != None) {
			printf("%c ", operatorToChar(&stack->tokens[i]));
		} else {
			printf("%.15Lg ", stack->tokens[i].data);
		}
	}

	printf("\n");
}
