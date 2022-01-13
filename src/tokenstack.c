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

static char operatorToChar(const Token *operator) {
	static_assert(NumOperators == 5, "Exhaustive handling of operators in operatorToChar");
	if (operator->type != Operator) {
		fprintf(stderr, "Error translating operator to character: Provided token is not an operator.");
		exit(2);
	}

	switch(*(enum Operator*)operator->data) {
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
	default:
		fprintf(stderr, "Error translating operator to character: Invalid operator '%d'.\n", *(enum Operator*)operator->data);
		exit(2);
	}
}

static char delimToChar(const Token *delim) {
	static_assert(NumDelims == 2, "Exhaustive handling of delimiters in delimToChar");
	if (delim->type != Delim) {
		fprintf(stderr, "Error translating delimiter to character: Provided token is not a delimiter.");
		exit(2);
	}

	switch(*(enum Delim*)delim->data) {
	case OpenParen:
		return '(';
	case CloseParen:
		return ')';
	default:
		fprintf(stderr, "Error translating delimiter to character: Invalid delimiter '%d'.\n", *(enum Delim*)delim->data);
		exit(2);
	}
}

void TokenStack_print(const TokenStack *stack) {
	static_assert(NumTypes == 5, "Exhaustive handling of token types in TokenStack_print");
	for (int i = 0; i < stack->length; i++) {
		switch (stack->tokens[i].type) {
		case Operator:
			printf("%c ", operatorToChar(&stack->tokens[i]));
			break;
		case Operand:
			printf("%.15Lg ", *(Operand_t*)stack->tokens[i].data);
			break;
		case Err:
			printf("Error %d ", *(Err_t*)stack->tokens[i].data);
			break;
		case Delim:
			printf("%c ", delimToChar(&stack->tokens[i]));
			break;
		case Null:
			printf("Null ");
			break;
		default:
			fprintf(stderr, "Error printing token stack: An invalid token was encountered.\n");
			exit(2);
		}
	}

	printf("\n");
}

Token Token_new(enum TokenType type) {
	static_assert(NumTypes == 5, "Exhaustive handling of token types in Token_new");
	Token newToken = {
		.type = type
	};

	switch (type) {
	case Operand:
		newToken.data = malloc(sizeof(Operand_t));
		break;
	case Operator:
		newToken.data = malloc(sizeof(enum Operator));
		break;
	case Delim:
		newToken.data = malloc(sizeof(enum Delim));
		break;
	case Err:
		newToken.data = malloc(sizeof(Err_t));
		break;
	case Null:
		newToken.data = NULL;
		break;
	default:
		fprintf(stderr, "Unable to create token: invalid type.\n");
		exit(2);
	}

	return newToken;
}

void Token_delete(Token *token) {
	static_assert(NumTypes == 5, "Exhaustive handling of token types in Token_delete");
	// This switch-case will only include special cases for freeing data.
	switch (token->type) {
	default:
		free(token->data);
	}

	// The token will be replaced with a null token to avoid undefined behavior.
	*token = Token_new(Null);
}

Token Token_throwError(Err_t errorlevel) {
	Token error = Token_new(Err);
	*(Err_t*) error.data = errorlevel;

	return error;
}
