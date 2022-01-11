#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokenstack.h"

TokenStack TokenStack_new() {
	TokenStack stack = {
		.length = 0,
		.top = NULL,
		.tokens = malloc(DEFAULT_LENGTH * sizeof(Token))
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

	if (stack->length % DEFAULT_LENGTH == 0) {
		Token *newArray = reallocarray(stack->tokens, stack->length + DEFAULT_LENGTH, sizeof(Token));

		if (newArray == NULL) {
			fprintf(stderr, "Out of memory error.\n");
			exit(1);
		} else {
			stack->tokens = newArray;
			stack->top = stack->tokens + stack->length - 1;
		}
	}

	*++stack->top = *data;
	stack->length++;
}

Token TokenStack_pop(TokenStack *stack) {
	Token poppedToken = *stack->top--;
	if (--stack->length % DEFAULT_LENGTH == 0 && stack->length > 0) {
		Token *newArray = reallocarray(stack->tokens, stack->length, sizeof(Token));

		if (newArray == NULL) {
			fprintf(stderr, "Error reducing size of the stack.\n");
			exit(1);
		} else {
			stack->tokens = newArray;
		}
	}

	return poppedToken;
}

Token TokenStack_peek(const TokenStack *stack) {
	return *stack->top;
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
	for (int i = 0; i < stack->length; i++) {
		if (stack->tokens[i].op != None) {
			printf("%c ", operatorToChar(&stack->tokens[i]));
		} else {
			printf("%.15Lg ", stack->tokens[i].data);
		}
	}

	printf("\n");
}

Token Token_throwError(int errorlevel) {
	Token error = {
		.op = Err,
		.data = errorlevel
	};

	return error;
}
