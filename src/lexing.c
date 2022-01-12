#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokenstack.h"
#include "lexing.h"

static void pushOperator(TokenStack *outputStack, const char *operator) {
	Token newOperator = Token_new(Operator);
	enum Operator *tokenData = (enum Operator*) newOperator.data;

	switch(*operator) {
	case '+':
		*tokenData = Add;
		break;
	case '-':
		*tokenData = Sub;
		break;
	case '*':
		*tokenData = Mul;
		break;
	case '/':
		*tokenData = Div;
		break;
	case '^':
		*tokenData = Exp;
		break;
	case '(':
		*tokenData = OpenParen;
		break;
	case ')':
		*tokenData = CloseParen;
		break;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", *operator);
		Token_delete(&newOperator);
		newOperator = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newOperator);
}

static void pushOperand(TokenStack *outputStack, Operand_t operand) {
	Token temp = Token_new(Operand);
	*(Operand_t*)temp.data = operand;

	TokenStack_push(outputStack, &temp);
}

TokenStack lexInput(char *input) {
	char *current = input;
	TokenStack outputStack = TokenStack_new();

	while (*current != '\0') {
		Token lastToken = Token_new(Null);
		if (outputStack.length > 0) lastToken = TokenStack_peek(&outputStack);
		if (lastToken.type == Err) {
			fprintf(stderr, "Encountered Error %d while lexing.\n", *(Err_t*)lastToken.data);
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
		} else {
			pushOperator(&outputStack, current++);
		}
	}

	return outputStack;
}
