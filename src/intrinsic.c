#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenstack.h"
#include "parsing.h"
#include "eval.h"
#include "intrinsic.h"

bool isIntrinsic(char *str) {
	return strncmp(str, "abs(", 4) == 0 ||
		   strncmp(str, "sqrt(", 5) == 0 ||
		   strncmp(str, "ln(", 3) == 0 ||
		   strncmp(str, "sin(", 4) == 0 ||
		   strncmp(str, "cos(", 4) == 0 ||
		   strncmp(str, "tan(", 4) == 0 ||
		   strncmp(str, "arcsin(", 7) == 0 ||
		   strncmp(str, "arccos(", 7) == 0 ||
		   strncmp(str, "arctan(", 7) == 0;
}

/* Accepts an expression wrapped in parenthesis and
 * finds the length between both parentheses */
int exprlen(char *str) {
	int hangingParenthesis = 0;
	int length = 0;

	// Get length of string wrapped in parentheses
	do {
		if (str[length] == '(') hangingParenthesis++;
		else if (str[length] == ')') hangingParenthesis--;
		length++;
	} while (hangingParenthesis > 0);

	return length;
}

// Accepts an expression wrapped in parenthesis and reduces it
Token_t reduceExpr(char *str, int length) {
	char temp[length + 1];
	strncpy(temp, str, length);
	temp[length] = '\0';
	TokenStack exprStack = parseInput(temp);
	Token_t result = evaluateOpStack(&exprStack);

	return result;
}