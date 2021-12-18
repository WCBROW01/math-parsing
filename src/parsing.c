#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "opdeque.h"

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

OpDeque parseInput(char *input) {
	char *current = input;

	OpDeque output = OpDeque_new();
	OpDeque opStack = OpDeque_new();

	// Account for leading +/- signs
	if (*current == '+' || *current == '-') {
		Op temp = {
			.isOperator = false,
			.data = strtol(current, &current, 10)
		};

		OpDeque_push(&output, &temp);
	}

	while (*current != '\0') {
		if (isdigit(*current)) {
			Op temp = {
				.isOperator = false,
				.data = strtol(current, &current, 10)
			};

			OpDeque_push(&output, &temp);
		} else if (*current == '(') {
			// If there is a digit before the parenthesis, imply multiplication.
			if (input != current && isdigit(*(current - 1))) {
				Op temp = {
					.isOperator = true,
					.data = MUL
				};

				OpDeque_push(&opStack, &temp);
			}

			Op temp = {
				.isOperator = true,
				.data = OPEN_PAREN
			};

			OpDeque_push(&opStack, &temp);
			current++;
		} else if (*current == ')') {
			// Pop everything within the parenthesis into the output stack
			while (OpDeque_peekBack(&opStack).data != OPEN_PAREN) {
				Op operator = OpDeque_popBack(&opStack);
				OpDeque_push(&output, &operator);
			}

			// Pop the opening parenthesis
			OpDeque_popBack(&opStack);

			// If there is a digit after the parenthesis, imply multiplication.
			if (isdigit(*(current + 1))) {
				Op temp = {
					.isOperator = true,
					.data = MUL
				};

				OpDeque_push(&opStack, &temp);
			}

			current++;
		} else {
			Op temp = {
				.isOperator = true,
				.data = parseOperator(*current++)
			};

			if (opStack.length == 0) OpDeque_push(&opStack, &temp);
			else if (opStack.length > 0) {
				/* If anything in the operator stack has a higher precedence and isn't a
				 * parenthesis, pop it from the stack and push it to the output. */
				while (opStack.length > 0 &&
					   getOperatorPrecedence(OpDeque_peekBack(&opStack)) < 3 &&
					   getOperatorPrecedence(OpDeque_peekBack(&opStack)) >=
					   getOperatorPrecedence(temp))
				{
					Op operator = OpDeque_popBack(&opStack);
					OpDeque_push(&output, &operator);
				}

				OpDeque_push(&opStack, &temp);
			}
		}
	}

	// Pop everything remaining in the operator stack into the output
	while (opStack.length > 0) {
		Op operator = OpDeque_popBack(&opStack);
		OpDeque_push(&output, &operator);
	}

	OpDeque_print(&output);
	OpDeque_delete(&opStack);

	return output;
}
