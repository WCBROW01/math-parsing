#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "opstack.h"

int getOperatorPrecedence(const Op operator) {
	switch(operator.data) {
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
		fprintf(stderr, "Invalid operator '%ld'.\n", operator.data);
		exit(2);
	}
}

long parseOperator(const char operator) {
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

OpStack parseInput(char *input) {
	char *current = input;
	int hangingParenthesis = 0;

	OpStack output = OpStack_new();
	OpStack operatorStack = OpStack_new();

	// Account for leading +/- signs
	if (*current == '+' || *current == '-') {
		Op temp = {
			.isOperator = false,
			.data = strtol(current, &current, 10)
		};

		OpStack_push(&output, &temp);
	}

	while (*current != '\0') {
		if (isdigit(*current)) {
			Op temp = {
				.isOperator = false,
				.data = strtol(current, &current, 10)
			};

			OpStack_push(&output, &temp);
		} else if (*current == '(') {
			// If there is a digit before the parenthesis, imply multiplication.
			if (input != current && isdigit(*(current - 1))) {
				Op temp = {
					.isOperator = true,
					.data = MUL
				};

				OpStack_push(&operatorStack, &temp);
			}

			Op temp = {
				.isOperator = true,
				.data = OPEN_PAREN
			};

			OpStack_push(&operatorStack, &temp);
			hangingParenthesis++;
			current++;
		} else if (*current == ')') {
			if (hangingParenthesis == 0) {
				printf("Invalid expression: You are closing one or more parenthesis that don't exist!\n");
				exit(3);
			}

			// Pop everything within the parenthesis into the output stack
			while (OpStack_peek(&operatorStack).data != OPEN_PAREN) {
				Op operator = OpStack_pop(&operatorStack);
				OpStack_push(&output, &operator);
			}

			// Pop the opening parenthesis
			OpStack_pop(&operatorStack);

			// If there is a digit after the parenthesis, imply multiplication.
			if (isdigit(*(current + 1))) {
				Op temp = {
					.isOperator = true,
					.data = MUL
				};

				OpStack_push(&operatorStack, &temp);
			}

			hangingParenthesis--;
			current++;
		} else {
			Op temp = {
				.isOperator = true,
				.data = parseOperator(*current++)
			};

			if (operatorStack.length == 0) OpStack_push(&operatorStack, &temp);
			else if (operatorStack.length > 0) {
				/* If anything in the operator stack has a higher precedence and isn't a
				 * parenthesis, pop it from the stack and push it to the output. */
				while (operatorStack.length > 0 &&
					   getOperatorPrecedence(OpStack_peek(&operatorStack)) < 3 &&
					   getOperatorPrecedence(OpStack_peek(&operatorStack)) >=
					   getOperatorPrecedence(temp))
				{
					Op operator = OpStack_pop(&operatorStack);
					OpStack_push(&output, &operator);
				}

				OpStack_push(&operatorStack, &temp);
			}
		}
	}

	if (hangingParenthesis > 0) {
		printf("Invalid expression: You have %d unclosed parenthesis.\n", hangingParenthesis);
		exit(3);
	}

	// Pop everything remaining in the operator stack into the output
	while (operatorStack.length > 0) {
		Op operator = OpStack_pop(&operatorStack);
		OpStack_push(&output, &operator);
	}

	OpStack_print(&output);
	OpStack_delete(&operatorStack);

	return output;
}
