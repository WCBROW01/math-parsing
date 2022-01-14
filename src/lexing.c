#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokenstack.h"
#include "lexing.h"

static_assert(NumOperators == 5, "Exhaustive handling of operators in ISOPERATOR");
#define ISOPERATOR(op) (op == '+' || op == '-' || op == '*' || op == '/' || op == '^')

static void pushOperator(TokenStack *outputStack, const char *operator) {
	static_assert(NumOperators == 5, "Exhaustive handling of operators in pushOperator");
	Token newOperator = {.type = Operator};

	switch(*operator) {
	case '+':
		newOperator.data.operator = Add;
		break;
	case '-':
		newOperator.data.operator = Sub;
		break;
	case '*':
		newOperator.data.operator = Mul;
		break;
	case '/':
		newOperator.data.operator = Div;
		break;
	case '^':
		newOperator.data.operator = Exp;
		break;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", *operator);
		newOperator = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newOperator);
}

static_assert(NumDelims == 2, "Exhaustive handling of delimiters in ISDELIM");
#define ISDELIM(delim) (delim == '(' || delim == ')')

static void pushDelim(TokenStack *outputStack, const char *delim) {
	static_assert(NumDelims == 2, "Exhaustive handling of delimiters in pushDelim");
	Token newDelim = {.type = Delim};

	switch(*delim) {
	case '(':
		newDelim.data.delim = OpenParen;
		break;
	case ')':
		newDelim.data.delim = CloseParen;
		break;
	default:
		fprintf(stderr, "Invalid delimiter '%c'.\n", *delim);
		newDelim = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newDelim);
}

static void pushOperand(TokenStack *outputStack, Operand_t operand) {
	Token temp = {
		.type = Operand,
		.data.operand = operand
	};

	TokenStack_push(outputStack, &temp);
}

TokenStack lexInput(char *input) {
	static_assert(NumTypes == 5, "Exhaustive handling of token types in lexInput");
	char *current = input;
	TokenStack outputStack = TokenStack_new();

	while (*current != '\0') {
		Token lastToken = {.type = Null};
		if (outputStack.length > 0) lastToken = TokenStack_peek(&outputStack);
		if (lastToken.type == Err) {
			fprintf(stderr, "Encountered Error %d while lexing.\n", lastToken.data.err);
			break;
		}

		if (isdigit(*current)) {
			pushOperand(&outputStack, strtold(current, &current));
		} else if (*current == ' ') {
			current++;
		} else if (*current == '+' || *current == '-') {
			// Checks if the + or - is an operator or part of an operand
			if (lastToken.type == Operator) {
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
