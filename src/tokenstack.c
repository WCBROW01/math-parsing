#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenstack.h"

static_assert(NUM_OPERATORS == 7, "Exhaustive handling of operators in OPERATOR_CHAR_TABLE");
const char *OPERATOR_STR_TABLE[NUM_OPERATORS] = {"=", "+", "-", "*", "/", "%", "^"};

static_assert(NUM_DELIMS == 3, "Exhaustive handling of delimiters in DELIM_CHAR_TABLE");
const char *DELIM_STR_TABLE[NUM_DELIMS] = {"(", ")", ","};

static_assert(NUM_INTRINSICS == 18, "Exhaustive handling of intrinsics in INTRINSIC_STR_TABLE");
const char *INTRINSIC_STR_TABLE[NUM_INTRINSICS] = {"abs", "sqrt", "cbrt", "ln", "log10", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "rand", "floor", "ceil", "ldexp", "min", "max"};

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
	assert(stack->length > 0);
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
	assert(stack->length > 0);
	return *stack->top;
}

void TokenStack_print(const TokenStack *stack) {
	static_assert(NUM_TYPES == 7, "Exhaustive handling of token types in TokenStack_print");
	for (int i = 0; i < stack->length; i++) {
		switch (stack->tokens[i].type) {
		case OPERATOR:
			printf("%s ", OPERATOR_STR_TABLE[stack->tokens[i].data.operator]);
			break;
		case OPERAND:
			printf("%.15Lg ", stack->tokens[i].data.operand);
			break;
		case ERR:
			printf("Error %d ", stack->tokens[i].data.err);
			break;
		case DELIM:
			printf("%s ", DELIM_STR_TABLE[stack->tokens[i].data.delim]);
			break;
		case INTRINSIC:
			printf("%s ", INTRINSIC_STR_TABLE[stack->tokens[i].data.intrinsic]);
			break;
		case VAR:
			printf("%s ", stack->tokens[i].data.var->name);
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
