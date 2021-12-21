#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenstack.h"
#include "intrinsic.h"
#include "eval.h"
#include "parsing.h"

/* Accepts a string containing an intrinsic and a pointer where the first index
 * after the end of intrinsic will be stored */
static Token_t parseIntrinsic(char *str, char **endp) {
	if (strncmp(str, "abs(", 4) == 0) {
		char *begin = str + 3;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Abs);
	} else if (strncmp(str, "sqrt(", 5) == 0) {
		char *begin = str + 4;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Sqrt);
	} else if (strncmp(str, "ln(", 3) == 0) {
		char *begin = str + 2;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Ln);
	} else if (strncmp(str, "sin(", 4) == 0) {
		char *begin = str + 3;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Sin);
	} else if (strncmp(str, "cos(", 4) == 0) {
		char *begin = str + 3;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Cos);
	} else if (strncmp(str, "tan(", 4) == 0) {
		char *begin = str + 3;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Tan);
	} else if (strncmp(str, "arcsin(", 7) == 0) {
		char *begin = str + 6;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Arcsin);
	} else if (strncmp(str, "arccos(", 7) == 0) {
		char *begin = str + 6;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Arccos);
	} else if (strncmp(str, "arctan(", 7) == 0) {
		char *begin = str + 6;
		int length = exprlen(begin);
		*endp = begin + length;
		return evaluateIntrinsic(reduceExpr(begin, length), Arctan);
	} else {
		assert(0 && "Attempted to parse an intrinsic that doesn't exist.");
	}
}

static int getOperatorPrecedence(const Token operator) {
	switch(operator.op) {
	case Add:
	case Sub:
		return 0;
	case Mul:
	case Div:
		return 1;
	case Exp:
		return 2;
	case OpenParen:
	case CloseParen:
		return 3;
	default:
		fprintf(stderr, "Invalid operator '%d'.\n", operator.op);
		exit(2);
	}
}

static Operator parseOperator(const char operator) {
	switch(operator) {
	case '+':
		return Add;
	case '-':
		return Sub;
	case '*':
		return Mul;
	case '/':
		return Div;
	case '^':
		return Exp;
	case '(':
		return OpenParen;
	case ')':
		return CloseParen;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", operator);
		exit(2);
	}
}

static void pushOperator(TokenStack *operatorStack, TokenStack *outputStack, Operator operator) {	
	Token newOperator = {
		.op = operator
	};

	if (operatorStack->length == 0) TokenStack_push(operatorStack, &newOperator);
	else if (operatorStack->length > 0) {
		/* If anything in the operator stack has a higher precedence and isn't a
		 * parenthesis, pop it from the stack and push it to the outputStack. */
		while (operatorStack->length > 0 &&
			   getOperatorPrecedence(TokenStack_peek(operatorStack)) < 3 &&
			   getOperatorPrecedence(TokenStack_peek(operatorStack)) >=
			   getOperatorPrecedence(newOperator))
		{
			Token temp = TokenStack_pop(operatorStack);
			TokenStack_push(outputStack, &temp);
		}

		TokenStack_push(operatorStack, &newOperator);
	}
}

static void pushOperand(TokenStack *outputStack, Token_t operand) {
	Token temp = {
		.op = None,
		.data = operand
	};

	TokenStack_push(outputStack, &temp);
}

TokenStack parseInput(char *input) {
	char *current = input;
	int hangingParenthesis = 0;
	int numOperators = 0;
	int numOperands = 0;

	TokenStack outputStack = TokenStack_new();
	TokenStack operatorStack = TokenStack_new();

	while (*current != '\0') {
		if (isdigit(*current)) {
			pushOperand(&outputStack, strtold(current, &current));
			numOperands++;
		} else if (*current == ' ') {
			current++;
		} else if (*current == '+' || *current == '-') {
			char *lastOp;
			if (input != current) {
				// Look for the last character that isn't whitespace
				lastOp = current - 1;
				while (*lastOp == ' ') lastOp--;
			} else lastOp = NULL; 

			// Checks if the + or - is an operator or part of an operand
			if (lastOp != NULL && (isdigit(*lastOp) || *lastOp == ')')) {			
				pushOperator(&operatorStack, &outputStack, parseOperator(*current++));
				numOperators++;
			} else {
				pushOperand(&outputStack, strtold(current, &current));
				numOperands++;
			}
		} else if (*current == '(') {
			if (input != current) {
				// Look for the last character that isn't whitespace
				char *lastOp = current - 1;
				while (*lastOp == ' ') lastOp--;

				// If there is a digit before the parenthesis, imply multiplication.
				if (isdigit(*lastOp)) {
					pushOperator(&operatorStack, &outputStack, Mul);
					numOperators++;
				}
			}
			pushOperator(&operatorStack, &outputStack, OpenParen);
			hangingParenthesis++;
			current++;
		} else if (*current == ')') {
			if (hangingParenthesis == 0) {
				printf("Invalid expression: You are closing one or more parenthesis that don't exist!\n");
				exit(3);
			} else { 
				char *lastOp = current - 1;
				while (*lastOp == ' ') lastOp--;

				if (*lastOp == '(') { 
					printf("Invalid expression: You have an empty set of parenthesis!\n");
					exit(3);
				}
			}

			// Pop everything within the parenthesis into the outputStack stack
			while (TokenStack_peek(&operatorStack).op != OpenParen) {
				Token operator = TokenStack_pop(&operatorStack);
				TokenStack_push(&outputStack, &operator);
			}

			// Pop the opening parenthesis
			TokenStack_pop(&operatorStack);

			// Look for the next character that isn't whitespace
			char *nextOp = current + 1;
			while (*nextOp == ' ') nextOp++;

			// If there is a digit after the parenthesis, imply multiplication.
			if (isdigit(*nextOp)) {
				pushOperator(&operatorStack, &outputStack, Mul);
				numOperators++;
			}

			hangingParenthesis--;
			current++;
		} else if (isIntrinsic(current)) {
			Token_t temp = parseIntrinsic(current, &current);
			pushOperand(&outputStack, temp);
			numOperands++;
		} else {
			pushOperator(&operatorStack, &outputStack, parseOperator(*current++));
		}
	}

	if (numOperators > numOperands - 1) {
		printf("Invalid expression: You have too many operators!\n");
		exit(4);
	} else if (hangingParenthesis > 0) {
		printf("Invalid expression: You have %d unclosed parenthesis.\n", hangingParenthesis);
		exit(3);
	}

	// Pop everything remaining in the operator stack into the outputStack
	while (operatorStack.length > 0) {
		Token operator = TokenStack_pop(&operatorStack);
		TokenStack_push(&outputStack, &operator);
	}

	TokenStack_delete(&operatorStack);

	return outputStack;
}
