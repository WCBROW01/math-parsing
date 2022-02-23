#include <assert.h>
#include <math.h>

#include "tokenstack.h"
#include "operator.h"

static Token operator_assign(Token a, Token b) {
	a.data.var->data = b.data.operand;
	return b;
}

static Token operator_add(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = a.data.operand + b.data.operand
	};
}

static Token operator_sub(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = a.data.operand - b.data.operand
	};
}

static Token operator_mul(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = a.data.operand * b.data.operand
	};
}

static Token operator_div(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = a.data.operand / b.data.operand
	};
}

static Token operator_mod(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = fmodl(a.data.operand, b.data.operand)
	};
}

static Token operator_pow(Token a, Token b) {
	return (Token){
		.type = OPERAND,
		.data.operand = powl(a.data.operand, b.data.operand)
	};
}

static_assert(NUM_OPERATORS == 7, "Exhaustive handling of operators in OPERATOR_FUNCS");
Token (*OPERATOR_FUNCS[NUM_OPERATORS])(Token, Token) = {operator_assign, operator_add, operator_sub, operator_mul, operator_div, operator_mod, operator_pow};