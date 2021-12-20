#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "opstack.h"
#include "eval.h"

static Op performOperation(const Op *operator, const Op *a, const Op *b) {
	Op temp = {
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

long double evaluateOpStack(OpStack *input) {
	if (input->length == 0) return NAN;

	OpStack evalStack = OpStack_new();

	for (int i = 0; i < input->length; i++) {
		if (input->ops[i].isOperator) {
			Op b = OpStack_pop(&evalStack);
			Op a = OpStack_pop(&evalStack);
			Op newOperand = performOperation(&input->ops[i], &a, &b);
			OpStack_push(&evalStack, &newOperand);
		} else {
			Op temp = {
				.isOperator = false,
				.data = input->ops[i].data
			};

			OpStack_push(&evalStack, &temp);
		}
	}

	long double result = OpStack_pop(&evalStack).data;
	OpStack_delete(&evalStack);

	return result;
}
