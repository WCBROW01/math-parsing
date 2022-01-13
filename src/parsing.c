#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenstack.h"
#include "parsing.h"

static int getOperatorPrecedence(const Token operator) {
	static_assert(NumOperators == 5, "Exhaustive handling of operators in getOperatorPrecedence");
	switch(*(enum Operator*)operator.data) {
	case Add:
	case Sub:
		return 0;
	case Mul:
	case Div:
		return 1;
	case Exp:
		return 2;
	default:
		assert(0 && "Attempted to get precedence of an operator that doesn't exist");
	}
}

static void pushOperator(TokenStack *operatorStack, TokenStack *outputStack, Token *newOperator) {
	if (operatorStack->length == 0) TokenStack_push(operatorStack, newOperator);
	else if (operatorStack->length > 0) {
		/* If anything in the operator stack has a higher precedence,
		 * pop it from the stack and push it to the outputStack. */
		while (operatorStack->length > 0 &&
			   TokenStack_peek(operatorStack).type == Operator &&
			   getOperatorPrecedence(TokenStack_peek(operatorStack)) >=
			   getOperatorPrecedence(*newOperator))
		{
			Token temp = TokenStack_pop(operatorStack);
			TokenStack_push(outputStack, &temp);
		}

		TokenStack_push(operatorStack, newOperator);
	}
}

static void pushError(TokenStack *outputStack, int errorlevel) {
	Token error = Token_throwError(errorlevel);
	TokenStack_push(outputStack, &error);
}

TokenStack parseTokens(TokenStack *input) {
	static_assert(NumTypes == 5, "Exhaustive handling of token types in parseTokens");
	Token *current = input->tokens;
	Token lastToken = Token_new(Null);
	int hangingParenthesis = 0;
	int numOperators = 0;
	int numOperands = 0;

	TokenStack outputStack = TokenStack_new();
	TokenStack operatorStack = TokenStack_new();

	for (int i = 0; i < input->length; i++) {
		switch (current->type) {
		case Operand:
			TokenStack_push(&outputStack, current);
			numOperands++;
			break;
		case Operator:
			pushOperator(&operatorStack, &outputStack, current);
			numOperators++;
			break;
		case Delim:
			if (*(enum Delim*)current->data == OpenParen) {
				if (lastToken.type == Operand) {
					/* If there is an operand before the parenthesis imply multiplication. */
					Token temp = Token_new(Operator);
					*(enum Operator*)temp.data = Mul;
					pushOperator(&operatorStack, &outputStack, &temp);
					numOperators++;
				}

				pushOperator(&operatorStack, &outputStack, current);
				hangingParenthesis++;
			} else {
				if (hangingParenthesis == 0) {
					printf("Invalid expression: You are closing one or more parenthesis that don't exist!\n");
					pushError(&outputStack, 3);
					goto destruct;
				} else {
					// Pop everything within the current set of delimiters into the output stack
					while (TokenStack_peek(&operatorStack).type != Delim) {
						Token operator = TokenStack_pop(&operatorStack);
						TokenStack_push(&outputStack, &operator);
					}

					// Pop and delete the opening parenthesis
					Token temp = TokenStack_pop(&operatorStack);
					Token_delete(&temp);

					if (*(enum Delim*)current->data == CloseParen) {
						if (lastToken.type == Delim) {
							printf("Invalid expression: You have a set of delimiters with no contents.\n");
							pushError(&outputStack, 3);
							goto destruct;
						} else if (current != input->top && (current + 1)->type != Operator) {
							// If there is not an operator after the parenthesis, imply multiplication.
							Token temp = Token_new(Operator);
							*(enum Operator*)temp.data = Mul;
							pushOperator(&operatorStack, &outputStack, &temp);
							numOperators++;
						}

						hangingParenthesis--;
					}
				}
			}
			break;
		case Err:
			fprintf(stderr, "Error %d was let through into the parsing phase. This should never happen.\n", *(Err_t*)current->data);
			exit(1);
		case Null:
			fprintf(stderr, "Null token enountered during parsing.\n");
			exit(1);
		default:
			fprintf(stderr, "Invalid type %d was encountered during parsing phase.\n", current->type);
			exit(1);
		}

		*current++ = lastToken;
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
