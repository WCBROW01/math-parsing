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
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, fabsl(operand.data.operand));
}

static void intrinsic_sqrt(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, sqrtl(operand.data.operand));
}

static void intrinsic_ln(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, logl(operand.data.operand));
}

static void intrinsic_sin(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, sinl(operand.data.operand));
}

static void intrinsic_cos(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, cosl(operand.data.operand));
}

static void intrinsic_tan(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, tanl(operand.data.operand));
}

static void intrinsic_arcsin(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, asinl(operand.data.operand));
}

static void intrinsic_arccos(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, acosl(operand.data.operand));
}

static void intrinsic_arctan(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, atanl(operand.data.operand));
}

static void intrinsic_rand(TokenStack *stack) {
	Token operand = TokenStack_pop(stack);
	pushOperand(stack, (Operand_t) rand() / RAND_MAX * operand.data.operand);
}

static_assert(NUM_INTRINSICS == 10, "Exhaustive handling of intrinsics in INTRINSIC_FUNCS");
void (*INTRINSIC_FUNCS[10])(TokenStack*) = {intrinsic_abs, intrinsic_sqrt, intrinsic_ln, intrinsic_sin, intrinsic_cos, intrinsic_tan, intrinsic_arcsin, intrinsic_arccos, intrinsic_arctan, intrinsic_rand};