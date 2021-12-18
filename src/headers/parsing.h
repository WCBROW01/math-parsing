#ifndef PARSING_H
#define PARSING_H

#include "opdeque.h"

extern int getOperatorPrecedence(const Op operator);
extern long parseOperator(const char *operator);
extern OpDeque parseInput(char *input);

#endif
