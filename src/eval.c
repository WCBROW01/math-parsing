#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "opstack.h"
#include "eval.h"

static Token performOperation(const Token *operator, const Token *a, const Token *b) {
	Token temp = {
		.isOperator = true
	};

	switch((int) operator->data) {
	case ADD:
		temp.data = a->data + b->data;
		break;
	case SUB:
		temp.data = a->data - b->data;
		break;
	case MUL:
		temp.data = a->data * b->data;
		break;
	case DIV:
		temp.data = a->data / b->data;
		break;
	case EXP:
		temp.data = powl(a->data, b->data);
		break;
	default:
		fprintf(stderr, "Invalid operator '%.15Lg'.\n", operator->data);
		exit(2);
	}

	return temp;
}

long double evaluateOpStack(TokenStack *input) {
	if (input->length == 0) return NAN;

	TokenStack evalStack = TokenStack_new();

	for (int i = 0; i < input->length; i++) {
		if (input->ops[i].isOperator) {
			Token b = TokenStack_pop(&evalStack);
			Token a = TokenStack_pop(&evalStack);
			Token newOperand = performOperation(&input->ops[i], &a, &b);
			TokenStack_push(&evalStack, &newOperand);
		} else {
			Token temp = {
				.isOperator = false,
				.data = input->ops[i].data
			};

			TokenStack_push(&evalStack, &temp);
		}
	}

	long double result = TokenStack_pop(&evalStack).data;
	TokenStack_delete(&evalStack);

	return result;
}
