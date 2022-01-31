#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tokentypes.h"
#include "tokenstack.h"
#include "lexing.h"

static bool substreq(char *str, const char *substr, char **endptr) {
	bool eq = true;
	size_t i;
	for (i = 0; eq; i++) {
		if (substr[i] == '\0') break;
		else if (str[i] != substr[i]) eq = false;
	}

	if (eq && endptr != NULL) *endptr = str + i;
	else if (endptr != NULL) *endptr = str;
	return eq;
}

static int searchTable(char *str, const char *table[], size_t length, char **endptr) {
	bool found = false;
	unsigned int index;
	for (index = 0; index < length; index++) {
		if (substreq(str, table[index], endptr)) {
			found = true;
			break;
		}
	}

	if (found) {
		return index;
	} else return -1;
}

TokenStack lexInput(char *input) {
	static_assert(NUM_TYPES == 6, "Exhaustive handling of token types in lexInput");
	char *current = input;
	TokenStack outputStack = TokenStack_new();
	Token lastToken = {.type = NULL_TOKEN};

	while (*current != '\0') {
		if (lastToken.type == ERR) {
			fprintf(stderr, "Encountered Error %d while lexing.\n", lastToken.data.err);
			break;
		}

		Token newToken; // Will be populated in this process

		if (isdigit(*current)) {
			newToken = (Token){
				.type = OPERAND,
				.data.operand = strtold(current, &current)
			};
		} else if (*current == ' ') {
			current++;
			continue;
		} else if (*current == '+' || *current == '-') {
			// Checks if the + or - is an operator or part of an operand
			if (lastToken.type == OPERATOR || lastToken.type == NULL_TOKEN || (lastToken.type == DELIM && lastToken.data.delim != CLOSE_PAREN)) {
				newToken = (Token){
					.type = OPERAND,
					.data.operand = strtold(current, &current)
				};
			} else {
				newToken = (Token){
					.type = OPERATOR,
					.data.operator = searchTable(current, OPERATOR_STR_TABLE, NUM_OPERATORS, &current)
				};
			}
		} else if ((newToken.data.operator = searchTable(current, OPERATOR_STR_TABLE, NUM_OPERATORS, &current)) != -1) {
			newToken.type = OPERATOR;
		} else if ((newToken.data.delim = searchTable(current, DELIM_STR_TABLE, NUM_DELIMS, &current)) != -1) {
			newToken.type = DELIM;
		} else if ((newToken.data.intrinsic = searchTable(current, INTRINSIC_STR_TABLE, NUM_INTRINSICS, &current)) != -1) {
			newToken.type = INTRINSIC;
		/* There is barely any benefit to doing anything other than substitution for constants,
		 * so we just won't. */
		} else if (substreq(current, "pi", &current)) {
			newToken = (Token){
				.type = OPERAND,
				.data.operand = M_PI
			};
		} else if (*current++ == 'e') {
			newToken = (Token){
				.type = OPERAND,
				.data.operand = M_E
			};
		} else {
			// Invalid token while lexing.
			fprintf(stderr, "Invalid input provided.\n");
			newToken = Token_throwError(2);
		}

		TokenStack_push(&outputStack, &newToken);
		lastToken = newToken;
	}

	return outputStack;
}
