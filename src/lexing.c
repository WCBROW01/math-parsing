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

	return found ? index : -1;
}

static int verifyVar(char *var) {
	if (var == NULL) return 0;
	char *temp = var;

	// Strip trailing spaces
	while (*++temp != '\0');
	while (*--temp == ' ') *temp = '\0';

	// Save length
	int length = temp - var + 1;

	// Check for any reserved characters in string (this looks stupid I hate it)
	temp = var;
	while (*temp != '\0') {
		if (*temp == ' ' ||
			searchTable(temp, OPERATOR_STR_TABLE, NUM_OPERATORS, NULL) != -1 ||
			searchTable(temp,    DELIM_STR_TABLE, NUM_DELIMS,    NULL) != -1
		) return 0;
		else temp++;
	}

	return length;
}

TokenStack lexInput(char *input) {
	static_assert(NUM_TYPES == 7, "Exhaustive handling of token types in lexInput");
	char *current = input;
	TokenStack outputStack = TokenStack_new();
	Token lastToken = {.type = NULL_TOKEN};
	bool variableAssigned = false;

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
		} else if (*current == '=') {
			if (!variableAssigned) {
				newToken = (Token){
					.type = OPERATOR,
					.data.operator = ASSIGN
				};

				variableAssigned = true;
				current++;
			} else {
				fprintf(stderr, "Attempted to assign multiple variables in one statement. This is not implemented.\n");
				newToken = Token_throwError(1);
			}
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
		} else if (*current == 'e') {
			current++;
			newToken = (Token){
				.type = OPERAND,
				.data.operand = M_E
			};
		// Try creating a variable token
		} else {
			int varLength;

			// Search for existing variables
			short varIndex = -1;
			for (short i = 0; i < numVars; i++) {
				if (substreq(current, varTable[i].name, NULL)) {
					varLength = strlen(varTable[i].name);
					varIndex = i;
					break;
				}
			}

			// Make token from existing variable
			if (varIndex != -1) {
				newToken = (Token){
					.type = VAR,
					.data.var = &varTable[varIndex]
				};

				current += varLength;
			// Create a new variable
			} else {
				for (varLength = 1; current[varLength] != '='; varLength++);
				char *varName = malloc(varLength + 1);
				strncpy(varName, current, varLength + 1);
				varName[varLength] = '\0';

				varLength = verifyVar(varName);
				if (varLength > 0) {
					varIndex = numVars++;
					varTable[varIndex] = (struct Var){
						.name = varName,
						.index = varIndex,
						.data = NAN
					};

					newToken = (Token){
						.type = VAR,
						.data.var = &varTable[varIndex]
					};

					current += varLength;
				} else {
					// Invalid token while lexing.
					free(varName);
					fprintf(stderr, "Invalid input provided.\n");
					newToken = Token_throwError(2);
				}
			}
		}

		TokenStack_push(&outputStack, &newToken);
		lastToken = newToken;
	}

	return outputStack;
}
