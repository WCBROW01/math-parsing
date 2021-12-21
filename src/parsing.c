#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tokenstack.h"

static int getOperatorPrecedence(const Token operator) {
	switch((int) operator.data) {
	case ADD:
	case SUB:
		return 0;
	case MUL:
	case DIV:
		return 1;
	case EXP:
		return 2;
	case OPEN_PAREN:
	case CLOSE_PAREN:
		return 3;
	default:
		fprintf(stderr, "Invalid operator '%.15Lg'.\n", operator.data);
		exit(2);
	}
}

static long double parseOperator(const char operator) {
	switch(operator) {
	case '+':
		return ADD;
	case '-':
		return SUB;
	case '*':
		return MUL;
	case '/':
		return DIV;
	case '^':
		return EXP;
	case '(':
		return OPEN_PAREN;
	case ')':
		return CLOSE_PAREN;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", operator);
		exit(2);
	}
}

static void pushOperator(TokenStack *operatorStack, TokenStack *outputStack, long double operator) {	
	Token newOperator = {
		.isOperator = true,
		.data = operator
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

TokenStack parseInput(char *input) {
	char *current = input;
	int hangingParenthesis = 0;

	TokenStack outputStack = TokenStack_new();
	TokenStack operatorStack = TokenStack_new();

	while (*current != '\0') {
		if (isdigit(*current)) {
			Token temp = {
				.isOperator = false,
				.data = strtold(current, &current)
			};

			TokenStack_push(&outputStack, &temp);
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
			} else {
				Token temp = {
					.isOperator = false,
					.data = strtold(current, &current)
				};

				TokenStack_push(&outputStack, &temp);
			}
		} else if (*current == '(') {
			if (input != current) {
				// Look for the last character that isn't whitespace
				char *lastOp = current - 1;
				while (*lastOp == ' ') lastOp--;

				// If there is a digit before the parenthesis, imply multiplication.
				if (isdigit(*lastOp)) {
					pushOperator(&operatorStack, &outputStack, MUL);
				}
			}
			pushOperator(&operatorStack, &outputStack, OPEN_PAREN);
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
			while (TokenStack_peek(&operatorStack).data != OPEN_PAREN) {
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
				pushOperator(&operatorStack, &outputStack, MUL);
			}

			hangingParenthesis--;
			current++;
		} else {
			pushOperator(&operatorStack, &outputStack, parseOperator(*current++));
		}
	}
	
	char *lastOp = current - 1;
	while (*lastOp == ' ') lastOp--;
	if (!isdigit(*lastOp) && *lastOp != ')') {
		printf("Invalid expression: You have no second operand for '%c'.\n", *lastOp);
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
