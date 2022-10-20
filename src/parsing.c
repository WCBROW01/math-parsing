#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenstack.h"
#include "parsing.h"

static_assert(NUM_OPERATORS == 13, "Exhaustive handling of operators in OPERATOR_PRECEDENCE");
static const int OPERATOR_PRECEDENCE[NUM_OPERATORS] = {2, 2, 3, 3, 3, 4, 1, 1, 1, 1, 1, 1, 0};

static void pushOperator(TokenStack *operatorStack, TokenStack *outputStack, Token newOperator) {
	/* If anything in the operator stack has a higher precedence,
	 * pop it from the stack and push it to the outputStack. */
	while (operatorStack->length > 0 &&
		   TokenStack_peek(operatorStack).type == OPERATOR &&
		   OPERATOR_PRECEDENCE[TokenStack_peek(operatorStack).data.operator] >=
		   OPERATOR_PRECEDENCE[newOperator.data.operator])
	{
		TokenStack_push(outputStack, TokenStack_pop(operatorStack));
	}

	TokenStack_push(operatorStack, newOperator);
}

#define PUSHMUL \
	do { \
		pushOperator(&operatorStack, &outputStack, (Token) { \
			.type = OPERATOR, \
			.data.operator = MUL \
		}); \
		numOperators++; \
	} while (0)

TokenStack parseTokens(TokenStack *input) {
	static_assert(NUM_TYPES == 7, "Exhaustive handling of token types in parseTokens");
	Token lastToken = {.type = NULL_TOKEN};
	int hangingParenthesis = 0;
	int numOperators = 0;
	int numOperands = 0;

	TokenStack outputStack = TokenStack_new();
	TokenStack operatorStack = TokenStack_new();

	for (int i = 0; i < input->length; ++i) {
		switch (input->tokens[i].type) {
		case OPERAND:
			TokenStack_push(&outputStack, input->tokens[i]);
			numOperands++;
			break;
		case OPERATOR:
			pushOperator(&operatorStack, &outputStack, input->tokens[i]);
			numOperators++;
			break;
		case DELIM:
			if (input->tokens[i].data.delim == OPEN_PAREN) {
				// If there is an operand before the parenthesis, imply multiplication.
				if (lastToken.type == OPERAND || lastToken.type == DELIM) PUSHMUL;
				TokenStack_push(&operatorStack, input->tokens[i]);
				hangingParenthesis++;
			} else {
				if (hangingParenthesis == 0) {
					TokenStack_push(&outputStack, Token_throwError(ERR_INVALID_EXPR, "You are closing one or more parenthesis that don't exist!"));
					goto destruct;
				} else {
					// Pop everything within the current set of delimiters into the output stack
					while (TokenStack_peek(&operatorStack).type != DELIM) {
						TokenStack_push(&outputStack, TokenStack_pop(&operatorStack));
					}

					if (input->tokens[i].data.delim == CLOSE_PAREN) {
						if (lastToken.type == DELIM && lastToken.data.delim == OPEN_PAREN) {
							TokenStack_push(&outputStack, Token_throwError(ERR_INVALID_EXPR, "You have a set of delimiters with no contents."));
							goto destruct;
						} else {
							// Pop the opening parenthesis
							TokenStack_pop(&operatorStack);

							// Check for intrinsic and pop it into the output stack if one exists
							if (operatorStack.length > 0 && TokenStack_peek(&operatorStack).type == INTRINSIC) {
								TokenStack_push(&outputStack, TokenStack_pop(&operatorStack));
							}

							// If there is not an operator after the parenthesis, imply multiplication.
							if (
								i != input->length - 1 &&
								input->tokens[i + 1].type != OPERATOR &&
								input->tokens[i + 1].type != DELIM
							) PUSHMUL;
							hangingParenthesis--;
						}
					// Required since multiple operands without operators will be pushed.
					} else if (input->tokens[i].data.delim == COMMA) numOperators++;
				}
			}
			break;
		case INTRINSIC:
			// If there is not an operator before the intrinsic, imply multiplication.
			if (lastToken.type != OPERATOR && lastToken.type != DELIM && lastToken.type != NULL_TOKEN) PUSHMUL;
			TokenStack_push(&operatorStack, input->tokens[i]);
			break;
		case VAR:
			TokenStack_push(&outputStack, input->tokens[i]);
			numOperands++;
			break;
		case ERR:
			fprintf(stderr, "Error %d was let through into the parsing phase. This should never happen.\n", input->tokens[i].data.err);
			exit(1);
		case NULL_TOKEN:
			fprintf(stderr, "Null token enountered during parsing.\n");
			exit(1);
		default:
			fprintf(stderr, "Invalid type %d was encountered during parsing phase.\n", input->tokens[i].type);
			exit(1);
		}

		lastToken = input->tokens[i];
	}

	if (numOperators != numOperands - 1) {
		TokenStack_push(&outputStack, Token_throwError(ERR_INVALID_EXPR, "You have too many operators!"));
		goto destruct;
	} else if (hangingParenthesis > 0) {
		TokenStack_push(&outputStack, Token_throwError(ERR_INVALID_EXPR, "There are unclosed parenthesis present."));
		goto destruct;
	}

	// Pop everything remaining in the operator stack into the outputStack
	while (operatorStack.length > 0) {
		TokenStack_push(&outputStack, TokenStack_pop(&operatorStack));
	}

	destruct:
	TokenStack_delete(&operatorStack);

	return outputStack;
}
