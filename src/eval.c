#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tokenstack.h"
#include "intrinsic.h"
#include "eval.h"

static void performOperation(TokenStack *evalStack) {
	static_assert(NUM_OPERATORS == 5, "Exhaustive handling of operators in performOperation");

	Token operator = TokenStack_pop(evalStack);
	Token b =		 TokenStack_pop(evalStack);
	Token a =		 TokenStack_pop(evalStack);

	Token result = {.type = OPERAND};

	switch(operator.data.operator) {
	case ADD:
		result.data.operand = a.data.operand + b.data.operand;
		break;
	case SUB:
		result.data.operand = a.data.operand - b.data.operand;
		break;
	case MUL:
		result.data.operand = a.data.operand * b.data.operand;
		break;
	case DIV:
		result.data.operand = a.data.operand / b.data.operand;
		break;
	case EXP:
		result.data.operand = powl(a.data.operand, b.data.operand);
		break;
	default:
		fprintf(stderr, "Invalid operator '%d'.\n", operator.data.operator);
		result.type = ERR;
		result.data.err = 2;
	}

	TokenStack_push(evalStack, &result);
}

static void evaluateIntrinsic(TokenStack *evalStack) {

	Token intrinsic = TokenStack_pop(evalStack);
	INTRINSIC_FUNCS[intrinsic.data.intrinsic](evalStack);
}

Operand_t evaluateTokenStack(TokenStack *input) {
	static_assert(NUM_TYPES == 6, "Exhaustive handling of token types in evaluateTokenStack");
	if (input->length == 0) return NAN;

	TokenStack evalStack = TokenStack_new();
	Token result;

	for (int i = 0; i < input->length; i++) {
		switch (input->tokens[i].type) {
		case OPERATOR:
			TokenStack_push(&evalStack, &input->tokens[i]);
			performOperation(&evalStack);
			break;
		case OPERAND:
			TokenStack_push(&evalStack, &input->tokens[i]);
			break;
		case INTRINSIC:
			TokenStack_push(&evalStack, &input->tokens[i]);
			evaluateIntrinsic(&evalStack);
			break;
		case ERR:
			printf("Error %d encountered during evaluation.", input->tokens[i].data.err);
			goto destruct;
		case NULL_TOKEN:
			fprintf(stderr, "Null token enountered during evaluation.\n");
			exit(1);
		default:
			printf("Invalid token encountered during evaluation.\n");
			Token errorToken = Token_throwError(5);
			TokenStack_push(&evalStack, &errorToken);
		}
	}

	destruct:
	result = TokenStack_pop(&evalStack);
	TokenStack_delete(&evalStack);

	return result.type == OPERAND ? result.data.operand : 0;
}
