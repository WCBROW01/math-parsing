#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokenstack.h"
#include "lexing.h"

static_assert(NUM_OPERATORS == 5, "Exhaustive handSubling of operators in ISOPERATOR");
#define ISOPERATOR(op) (op == '+' || op == '-' || op == '*' || op == '/' || op == '^')

static void pushOperator(TokenStack *outputStack, const char *operator) {
	static_assert(NUM_OPERATORS == 5, "Exhaustive handling of operators in pushOperator");
	Token newOperator = {.type = OPERATOR};

	switch(*operator) {
	case '+':
		newOperator.data.operator = ADD;
		break;
	case '-':
		newOperator.data.operator = SUB;
		break;
	case '*':
		newOperator.data.operator = MUL;
		break;
	case '/':
		newOperator.data.operator = DIV;
		break;
	case '^':
		newOperator.data.operator = EXP;
		break;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", *operator);
		newOperator = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newOperator);
}

static_assert(NUM_DELIMS == 2, "Exhaustive handling of delimiters in ISDELIM");
#define ISDELIM(delim) (delim == '(' || delim == ')')

static void pushDelim(TokenStack *outputStack, const char *delim) {
	static_assert(NUM_DELIMS == 2, "Exhaustive handling of delimiters in pushDelim");
	Token newDelim = {.type = DELIM};

	switch(*delim) {
	case '(':
		newDelim.data.delim = OPEN_PAREN;
		break;
	case ')':
		newDelim.data.delim = CLOSE_PAREN;
		break;
	default:
		fprintf(stderr, "Invalid delimiter '%c'.\n", *delim);
		newDelim = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newDelim);
}

static void pushOperand(TokenStack *outputStack, Operand_t operand) {
	Token temp = {
		.type = OPERAND,
		.data.operand = operand
	};

	TokenStack_push(outputStack, &temp);
}

TokenStack lexInput(char *input) {
	static_assert(NUM_TYPES == 5, "Exhaustive handling of token types in lexInput");
	char *current = input;
	TokenStack outputStack = TokenStack_new();

	while (*current != '\0') {
		Token lastToken = {.type = NULL_TOKEN};
		if (outputStack.length > 0) lastToken = TokenStack_peek(&outputStack);
		if (lastToken.type == ERR) {
			fprintf(stderr, "Encountered Error %d while lexing.\n", lastToken.data.err);
			break;
		}

		if (isdigit(*current)) {
			pushOperand(&outputStack, strtold(current, &current));
		} else if (*current == ' ') {
			current++;
		} else if (*current == '+' || *current == '-') {
			// Checks if the + or - is an operator or part of an operand
			if (lastToken.type == OPERATOR) {
				pushOperand(&outputStack, strtold(current, &current));
			} else {
				pushOperator(&outputStack, current++);
			}
		} else if (ISOPERATOR(*current)) {
			pushOperator(&outputStack, current++);
		} else if (ISDELIM(*current)) {
			pushDelim(&outputStack, current++);
		} else {
			assert(0 && "Unreachable");
		}
	}

	return outputStack;
}
