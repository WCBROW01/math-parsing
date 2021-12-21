#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tokenstack.h"
#include "intrinsic.h"
#include "eval.h"

static Token performOperation(const Token *operator, const Token *a, const Token *b) {
	Token temp = {
		.op = None
	};

	switch(operator->op) {
	case Add:
		temp.data = a->data + b->data;
		break;
	case Sub:
		temp.data = a->data - b->data;
		break;
	case Mul:
		temp.data = a->data * b->data;
		break;
	case Div:
		temp.data = a->data / b->data;
		break;
	case Exp:
		temp.data = powl(a->data, b->data);
		break;
	default:
		fprintf(stderr, "Invalid operator '%d'.\n", operator->op);
		exit(2);
	}

	return temp;
}

Token_t evaluateIntrinsic(Token_t token, Intrinsic intrinsic) {
	switch(intrinsic) {
	case Abs:
		return fabsl(token);
	case Sqrt:
		return sqrtl(token);
	case Ln:
		return logl(token);
	case Sin:
		return sinl(token);
	case Cos:
		return cosl(token);
	case Tan:
		return tanl(token);
	case Arcsin:
		return asinl(token);
	case Arccos:
		return acosl(token);
	case Arctan:
		return atanl(token);
	default:
		assert(0 && "Intrinsic doesn't exist");
	}
}

Token_t evaluateOpStack(TokenStack *input) {
	if (input->length == 0) return NAN;

	TokenStack evalStack = TokenStack_new();

	for (int i = 0; i < input->length; i++) {
		if (input->tokens[i].op != None) {
			Token b = TokenStack_pop(&evalStack);
			Token a = TokenStack_pop(&evalStack);
			Token newOperand = performOperation(&input->tokens[i], &a, &b);
			TokenStack_push(&evalStack, &newOperand);
		} else {
			Token temp = {
				.op = None,
				.data = input->tokens[i].data
			};

			TokenStack_push(&evalStack, &temp);
		}
	}

	Token_t result = TokenStack_pop(&evalStack).data;
	TokenStack_delete(&evalStack);

	return result;
}
