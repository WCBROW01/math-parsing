#ifndef INTRINSIC_H
#define INTRINSIC_H

#include <stdbool.h>
#include "tokenstack.h"

typedef enum Intrinsic {
	Abs,
	Sqrt,
	Ln,
	Sin,
	Cos,
	Tan,
	Arcsin,
	Arccos,
	Arctan
} Intrinsic;

extern bool isIntrinsic(char *str);

/* Accepts an expression wrapped in parenthesis and
 * finds the length between both parentheses */
extern int exprlen(char *str);

// Accepts an expression wrapped in parenthesis and reduces it
extern Token_t reduceExpr(char *str, int length);

#endif