#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenstack.h"
#include "parsing.h"

static int getOperatorPrecedence(const Token operator) {
	static_assert(NUM_OPERATORS == 5, "Exhaustive handling of operators in getOperatorPrecedence");
	switch(operator.data.operator) {
	case ADD:
	case SUB:
		return 0;
	case MUL:
	case DIV:
		return 1;
	case EXP:
		return 2;
	default:
		assert(0 && "Attempted to get precedence of an operator that doesn't exist");
	}
}

static void pushOperator(TokenStack *operatorStack, TokenStack *outputStack, Token *newOperator) {
	/* If anything in the operator stack has a higher precedence,
	 * pop it from the stack and push it to the outputStack. */
	while (operatorStack->length > 0 &&
		   TokenStack_peek(operatorStack).type == OPERATOR &&
		   getOperatorPrecedence(TokenStack_peek(operatorStack)) >=
		   getOperatorPrecedence(*newOperator))
	{
		Token temp = TokenStack_pop(operatorStack);
		TokenStack_push(outputStack, &temp);
	}

	TokenStack_push(operatorStack, newOperator);
}

static void pushError(TokenStack *outputStack, int errorlevel) {
	Token error = Token_throwError(errorlevel);
	TokenStack_push(outputStack, &error);
}

TokenStack parseTokens(TokenStack *input) {
	static_assert(NUM_TYPES == 6, "Exhaustive handling of token types in parseTokens");
	Token *current = input->tokens;
	Token lastToken = {.type = NULL_TOKEN};
	int hangingParenthesis = 0;
	int numOperators = 0;
	int numOperands = 0;

	TokenStack outputStack = TokenStack_new();
	TokenStack operatorStack = TokenStack_new();

	for (int i = 0; i < input->length; i++) {
		switch (current->type) {
		case OPERAND:
			TokenStack_push(&outputStack, current);
			numOperands++;
			break;
		case OPERATOR:
			pushOperator(&operatorStack, &outputStack, current);
			numOperators++;
			break;
		case DELIM:
			if (current->data.delim == OPEN_PAREN) {
				if (lastToken.type == OPERAND || lastToken.type == DELIM) {
					/* If there is an operand before the parenthesis imply multiplication. */
					Token temp = {
						.type = OPERATOR,
						.data.operator = MUL
					};

					pushOperator(&operatorStack, &outputStack, &temp);
					numOperators++;
				}

				TokenStack_push(&operatorStack, current);
				hangingParenthesis++;
			} else {
				if (hangingParenthesis == 0) {
					printf("Invalid expression: You are closing one or more parenthesis that don't exist!\n");
					pushError(&outputStack, 3);
					goto destruct;
				} else {
					// Pop everything within the current set of delimiters into the output stack
					while (TokenStack_peek(&operatorStack).type != DELIM) {
						Token operator = TokenStack_pop(&operatorStack);
						TokenStack_push(&outputStack, &operator);
					}

					// Pop the opening parenthesis
					TokenStack_pop(&operatorStack);

					// Check for intrinsic and pop it into the output stack if one exists
					if (TokenStack_peek(&operatorStack).type == INTRINSIC) {
						Token intrinsic = TokenStack_pop(&operatorStack);
						TokenStack_push(&outputStack, &intrinsic);
					}

					if (current->data.delim == CLOSE_PAREN) {
						if (lastToken.type == DELIM && lastToken.data.delim == OPEN_PAREN) {
							printf("Invalid expression: You have a set of delimiters with no contents.\n");
							pushError(&outputStack, 3);
							goto destruct;
						} else if (current != input->top && (current + 1)->type != OPERATOR && (current + 1)->type != DELIM) {
							// If there is not an operator after the parenthesis, imply multiplication.
							Token temp = {
								.type = OPERATOR,
								.data.operator = MUL
							};

							pushOperator(&operatorStack, &outputStack, &temp);
							numOperators++;
						}

						hangingParenthesis--;
					}
				}
			}
			break;
		case INTRINSIC:
			if (lastToken.type != OPERATOR && lastToken.type != DELIM && lastToken.type != NULL_TOKEN) {
				// If there is not an operator before the intrinsic, imply multiplication.
				Token temp = {
					.type = OPERATOR,
					.data.operator = MUL
				};

				pushOperator(&operatorStack, &outputStack, &temp);
				numOperators++;
			}

			TokenStack_push(&operatorStack, current);
			break;
		case ERR:
			fprintf(stderr, "Error %d was let through into the parsing phase. This should never happen.\n", current->data.err);
			exit(1);
		case NULL_TOKEN:
			fprintf(stderr, "Null token enountered during parsing.\n");
			exit(1);
		default:
			fprintf(stderr, "Invalid type %d was encountered during parsing phase.\n", current->type);
			exit(1);
		}

		lastToken = *current++;
	}

	if (numOperators != numOperands - 1) {
		printf("Invalid expression: You have too many operators!\n");
		printf("Operands: %d, Operators: %d\n", numOperands, numOperators);
		pushError(&outputStack, 4);
		goto destruct;
	} else if (hangingParenthesis > 0) {
		printf("Invalid expression: You have %d unclosed parenthesis.\n", hangingParenthesis);
		pushError(&outputStack, 3);
		goto destruct;
	}

	// Pop everything remaining in the operator stack into the outputStack
	while (operatorStack.length > 0) {
		Token operator = TokenStack_pop(&operatorStack);
		TokenStack_push(&outputStack, &operator);
	}

	destruct:
	TokenStack_delete(&operatorStack);

	return outputStack;
}
