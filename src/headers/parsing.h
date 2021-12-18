#ifndef PARSING_H
#define PARSING_H

#include "opstack.h"

extern int getOperatorPrecedence(const Op operator);
extern long parseOperator(const char *operator);
extern OpStack parseInput(char *input);

#endif
