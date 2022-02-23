#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tokenstack.h"
#include "operator.h"
#include "intrinsic.h"
#include "eval.h"

Operand_t evaluateTokenStack(TokenStack *input) {
	static_assert(NUM_TYPES == 7, "Exhaustive handling of token types in evaluateTokenStack");
	if (input->length == 0) return NAN;

	TokenStack evalStack = TokenStack_new();
	Token result;

	for (int i = 0; i < input->length; i++) {
		switch (input->tokens[i].type) {
		case OPERATOR:
			Token b = TokenStack_pop(&evalStack);
			Token a = TokenStack_pop(&evalStack);
			Token result = OPERATOR_FUNCS[input->tokens[i].data.operator](a, b);
			if (a.type != VAR) TokenStack_push(&evalStack, &result);
			break;
		case OPERAND:
			TokenStack_push(&evalStack, &input->tokens[i]);
			break;
		case INTRINSIC:
			INTRINSIC_FUNCS[input->tokens[i].data.intrinsic](&evalStack);
			break;
		case VAR:
			// Token is being used for assignment
			if (i == 0 && input->top->type == OPERATOR && input->top->data.operator == ASSIGN) {
				TokenStack_push(&evalStack, &input->tokens[i]);
			// Token is being used as an operand
			} else {
				Token operand = {
					.type = OPERAND,
					.data.operand = input->tokens[i].data.var->data
				};

				TokenStack_push(&evalStack, &operand);
			}
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
			fprintf(stderr, "Invalid token encountered during evaluation.\n");
			Token errorToken = Token_throwError(5);
			TokenStack_push(&evalStack, &errorToken);
		}
	}

	destruct:
	result = TokenStack_pop(&evalStack);
	TokenStack_delete(&evalStack);

	if (result.type == OPERAND) return result.data.operand;
	else return 0;
}
