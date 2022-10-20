#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tokenstack.h"

static_assert(NUM_OPERATORS == 13, "Exhaustive handling of operators in OPERATOR_STR_TABLE");
const char *OPERATOR_STR_TABLE[NUM_OPERATORS] = {"+", "-", "*", "/", "%", "^", "==", "!=", "<=", ">=", "<", ">", "="};

static_assert(NUM_DELIMS == 3, "Exhaustive handling of delimiters in DELIM_STR_TABLE");
const char *DELIM_STR_TABLE[NUM_DELIMS] = {"(", ")", ","};

static_assert(NUM_INTRINSICS == 19, "Exhaustive handling of intrinsics in INTRINSIC_STR_TABLE");
const char *INTRINSIC_STR_TABLE[NUM_INTRINSICS] = {"abs", "sqrt", "cbrt", "ln", "log10", "log2", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "rand", "floor", "ceil", "ldexp", "min", "max"};

static_assert(NUM_ERRORS == 7, "Exhaustive handling of errors in ERR_STR_TABLE");
const char *ERR_STR_TABLE[NUM_ERRORS] = {
	"Unable to create new variable \"%s\".\nDoes it use reserved words or already exist?\n",
	"Invalid input provided.\n",
	"Invalid token encountered during evaluation.\n",
	"Attempted to reassign a constant.\n",
	"Failed to allocate memory for new variable %s!\n",
	"%s: This is not implemented.\n",
	"Invalid expression: %s\n",
};

TokenStack TokenStack_new() {
	TokenStack stack = {
		.length = 0,
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

void TokenStack_push(TokenStack *stack, const Token data) {
	if (stack->length % DEFAULT_LENGTH == 0) {
		Token *newArray = reallocarray(stack->tokens, stack->length + DEFAULT_LENGTH, sizeof(Token));

		if (newArray == NULL) {
			fprintf(stderr, "Out of memory error.\n");
			exit(1);
		} else {
			stack->tokens = newArray;
		}
	}

	stack->tokens[stack->length++] = data;
}

Token TokenStack_pop(TokenStack *stack) {
	assert(stack->length > 0);
	Token poppedToken = stack->tokens[stack->length-- - 1];
	if (stack->length % DEFAULT_LENGTH == 0 && stack->length > 0) {
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
	return stack->tokens[stack->length - 1];
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

Token Token_throwError(enum Err errorlevel, ...) {
	if (errorlevel < 0 || errorlevel >= NUM_ERRORS) {
		fprintf(stderr, "Invalid error %d encountered.\n", errorlevel);
		exit(1);
	}
	
	va_list ap;
	va_start(ap, errorlevel);
	vfprintf(stderr, ERR_STR_TABLE[errorlevel], ap);
	va_end(ap);
	
	Token error = {
		.type = ERR,
		.data.err = errorlevel
	};

	return error;
}
