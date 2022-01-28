#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tokenstack.h"
#include "intrinsic.h"

static void pushOperand(TokenStack *stack, Operand_t operand) {
	Token newToken = {
		.type = OPERAND,
		.data.operand = operand
	};
	TokenStack_push(stack, &newToken);
}

static void intrinsic_abs(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, fabsl(x.data.operand));
}

static void intrinsic_sqrt(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, sqrtl(x.data.operand));
}

static void intrinsic_ln(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, logl(x.data.operand));
}

static void intrinsic_sin(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, sinl(x.data.operand));
}

static void intrinsic_cos(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, cosl(x.data.operand));
}

static void intrinsic_tan(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, tanl(x.data.operand));
}

static void intrinsic_asin(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, asinl(x.data.operand));
}

static void intrinsic_acos(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, acosl(x.data.operand));
}

static void intrinsic_atan(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, atanl(x.data.operand));
}

static void intrinsic_atan2(TokenStack *stack) {
	Token y = TokenStack_pop(stack);
	Token x = TokenStack_pop(stack);
	pushOperand(stack, atan2l(x.data.operand, y.data.operand));
}

static void intrinsic_rand(TokenStack *stack) {
	Token upper_bound = TokenStack_pop(stack);
	pushOperand(stack, (Operand_t) rand() / RAND_MAX * upper_bound.data.operand);
}

static void intrinsic_floor(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, floorl(x.data.operand));
}

static void intrinsic_ceil(TokenStack *stack) {
	Token x = TokenStack_pop(stack);
	pushOperand(stack, ceill(x.data.operand));
}

static void intrinsic_ldexp(TokenStack *stack) {
	Token exp = TokenStack_pop(stack);
	Token x =	TokenStack_pop(stack);
	pushOperand(stack, ldexpl(x.data.operand, (int) exp.data.operand));
}

static_assert(NUM_INTRINSICS == 14, "Exhaustive handling of intrinsics in INTRINSIC_FUNCS");
void (*INTRINSIC_FUNCS[14])(TokenStack*) = {intrinsic_abs, intrinsic_sqrt, intrinsic_ln, intrinsic_sin, intrinsic_cos, intrinsic_tan, intrinsic_asin, intrinsic_acos, intrinsic_atan, intrinsic_atan2, intrinsic_rand, intrinsic_floor, intrinsic_ceil, intrinsic_ldexp};
