#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tokentypes.h"
#include "tokenstack.h"
#include "intrinsic.h"
#include "eval.h"

static Token operator_set(Token a, Token b) {
	if (b.type == OPERAND) a.data.var->data = b.data.operand;
	else if (b.type == VAR) a.data.var->data = b.data.var->data;

	return b;
}

static Token operator_add(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = a_value + b_value
	};
}

static Token operator_sub(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = a_value - b_value
	};
}

static Token operator_mul(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = a_value * b_value
	};
}

static Token operator_div(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = a_value / b_value
	};
}

static Token operator_mod(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = fmodl(a_value, b_value)
	};
}

static Token operator_pow(Token a, Token b) {
	Operand_t a_value, b_value;

	assert(a.type == OPERAND || a.type == VAR);
	if (a.type == OPERAND) a_value = a.data.operand;
	else if (a.type == VAR) a_value = a.data.var->data;

	assert(b.type == OPERAND || b.type == VAR);
	if (b.type == OPERAND) b_value = b.data.operand;
	else if (b.type == VAR) b_value = b.data.var->data;

	return (Token){
		.type = OPERAND,
		.data.operand = powl(a_value, b_value)
	};
}

static_assert(NUM_OPERATORS == 7, "Exhaustive handling of operators in OPERATOR_FUNCS");
static Token (*OPERATOR_FUNCS[NUM_OPERATORS])(Token, Token) = {operator_set, operator_add, operator_sub, operator_mul, operator_div, operator_mod, operator_pow};

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
			TokenStack_push(&evalStack, &input->tokens[i]);
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

	if (result.type == OPERAND) return result.data.operand;
	else if (result.type == VAR) return result.data.var->data;
	else return 0;
}
