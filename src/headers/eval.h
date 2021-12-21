#ifndef EVAL_H
#define EVAL_H

#include "tokenstack.h"
#include "intrinsic.h"

extern Token_t evaluateOpStack(TokenStack *input);
extern Token_t evaluateIntrinsic(Token_t token, Intrinsic intrinsic);

#endif
