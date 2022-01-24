#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tokenstack.h"
#include "eval.h"

static void performOperation(TokenStack *evalStack) {
	static_assert(NUM_OPERATORS == 5, "Exhaustive handling of operators in performOperation");

	Token operator = TokenStack_pop(evalStack);
	Token b =		 TokenStack_pop(evalStack);
	Token a =		 TokenStack_pop(evalStack);

	Token temp = {
		.type = OPERAND
	};

	switch(operator.data.operator) {
	case ADD:
		temp.data.operand = a.data.operand + b.data.operand;
		break;
	case SUB:
		temp.data.operand = a.data.operand - b.data.operand;
		break;
	case MUL:
		temp.data.operand = a.data.operand * b.data.operand;
		break;
	case DIV:
		temp.data.operand = a.data.operand / b.data.operand;
		break;
	case EXP:
		temp.data.operand = powl(a.data.operand, b.data.operand);
		break;
	default:
		fprintf(stderr, "Invalid operator '%d'.\n", operator.data.operator);
		temp.type = ERR;
		temp.data.err = 2;
	}

	TokenStack_push(evalStack, &temp);
}

/*
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
*/

Operand_t evaluateTokenStack(TokenStack *input) {
	static_assert(NUM_TYPES == 5, "Exhaustive handling of token types in evaluateTokenStack");
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
