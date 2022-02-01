#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tokentypes.h"
#include "tokenstack.h"
#include "intrinsic.h"
#include "eval.h"

static Operand_t operator_add(Operand_t a, Operand_t b) {return a + b;}
static Operand_t operator_sub(Operand_t a, Operand_t b) {return a - b;}
static Operand_t operator_mul(Operand_t a, Operand_t b) {return a * b;}
static Operand_t operator_div(Operand_t a, Operand_t b) {return a / b;}

static_assert(NUM_OPERATORS == 6, "Exhaustive handling of operators in OPERATOR_FUNCS");
static Operand_t (*OPERATOR_FUNCS[NUM_OPERATORS])(Operand_t, Operand_t) = {operator_add, operator_sub, operator_mul, operator_div, fmodl, powl};

Operand_t evaluateTokenStack(TokenStack *input) {
	static_assert(NUM_TYPES == 6, "Exhaustive handling of token types in evaluateTokenStack");
	if (input->length == 0) return NAN;

	TokenStack evalStack = TokenStack_new();
	Token result;

	for (int i = 0; i < input->length; i++) {
		switch (input->tokens[i].type) {
		case OPERATOR:
			Token b = TokenStack_pop(&evalStack);
			Token a = TokenStack_pop(&evalStack);
			Token result = {
				.type = OPERAND,
				.data.operand = OPERATOR_FUNCS[input->tokens[i].data.operator](a.data.operand, b.data.operand)
			};

			TokenStack_push(&evalStack, &result);
			break;
		case OPERAND:
			TokenStack_push(&evalStack, &input->tokens[i]);
			break;
		case INTRINSIC:
			INTRINSIC_FUNCS[input->tokens[i].data.intrinsic](&evalStack);
			break;
		case DELIM:
			// Do nothing, these are skipped in RPN.
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
