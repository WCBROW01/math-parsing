#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenstack.h"

static_assert(NUM_INTRINSICS == 9, "Exhaustive handling of intrinsics in INTRINSIC_STR");
static const char *INTRINSIC_STR[9] = {"abs", "sqrt", "ln", "sin", "cos", "tan", "arcsin", "arccos", "arctan"};

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

static char operatorToChar(const Token *operator) {
	static_assert(NUM_OPERATORS == 5, "Exhaustive handling of operators in operatorToChar");
	if (operator->type != OPERATOR) {
		fprintf(stderr, "Error translating operator to character: Provided token is not an operator.");
		exit(2);
	}

	switch(operator->data.operator) {
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
	default:
		fprintf(stderr, "Error translating operator to character: Invalid operator '%d'.\n", operator->data.operator);
		exit(2);
	}
}

static char delimToChar(const Token *delim) {
	static_assert(NUM_DELIMS == 2, "Exhaustive handling of delimiters in delimToChar");
	if (delim->type != DELIM) {
		fprintf(stderr, "Error translating delimiter to character: Provided token is not a delimiter.");
		exit(2);
	}

	switch(delim->data.delim) {
	case OPEN_PAREN:
		return '(';
	case CLOSE_PAREN:
		return ')';
	default:
		fprintf(stderr, "Error translating delimiter to character: Invalid delimiter '%d'.\n", delim->data.delim);
		exit(2);
	}
}

void TokenStack_print(const TokenStack *stack) {
	static_assert(NUM_TYPES == 6, "Exhaustive handling of token types in TokenStack_print");
	for (int i = 0; i < stack->length; i++) {
		switch (stack->tokens[i].type) {
		case OPERATOR:
			printf("%c ", operatorToChar(&stack->tokens[i]));
			break;
		case OPERAND:
			printf("%.15Lg ", stack->tokens[i].data.operand);
			break;
		case ERR:
			printf("Error %d ", stack->tokens[i].data.err);
			break;
		case DELIM:
			printf("%c ", delimToChar(&stack->tokens[i]));
			break;
		case INTRINSIC:
			printf("%s ", INTRINSIC_STR[stack->tokens[i].data.intrinsic]);
			break;
		case NULL_TOKEN:
			printf("Null ");
			break;
		default:
			fprintf(stderr, "Error printing token stack: An invalid token was encountered.\n");
			exit(2);
		}
	}

	printf("\n");
}

Token Token_throwError(Err_t errorlevel) {
	Token error = {
		.type = ERR,
		.data.err = errorlevel
	};

	return error;
}
