#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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

static struct Var *searchVars(char *str, char **endptr) {
	bool found = false;
	ssize_t index;

	for (index = numVars - 1; index >= 0; index--) {
		if (substreq(str, varTable[index].name, endptr)) {
			found = true;
			break;
		}
	}

	return found ? &varTable[index] : NULL;
}

static bool verifyVar(char *var) {
	if (var == NULL) return false;
	else for (short i = 0; i < numVars; i++) {
		if (strcmp(var, varTable[i].name) == 0) return false;
	}

	// Check if the variable name matches any intrinsics
	for (size_t i = 0; i < NUM_INTRINSICS; i++) {
		if (strcmp(var, INTRINSIC_STR_TABLE[i]) == 0) return false;
	}

	// Check for any reserved characters in string (this looks stupid I hate it)
	while (*var != '\0') {
		if (searchTable(var,  OPERATOR_STR_TABLE, NUM_OPERATORS,  NULL) != -1 ||
			searchTable(var,     DELIM_STR_TABLE, NUM_DELIMS,     NULL) != -1
		) return false;
		else var++;
	}

	return true;
}

int compareVars(const void *a, const void *b) {
	struct Var *a_cast = (struct Var*) a;
	struct Var *b_cast = (struct Var*) b;

	return strcmp(a_cast->name, b_cast->name);
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
		} else if (substreq(current, "let ", &current)) {
			size_t varLength;

			while (*current == ' ') current++; // Skip spaces
			for (varLength = 1; current[varLength] != ' '; varLength++);
			char *varName = malloc(varLength + 1);
			strncpy(varName, current, varLength + 1);
			varName[varLength] = '\0';

			if (verifyVar(varName)) {
				short varIndex = numVars++;
				varTable[varIndex] = (struct Var){
					.name = varName,
					.index = varIndex,
					.data = 0
				};

				newToken = (Token){
					.type = VAR,
					.data.var = &varTable[varIndex]
				};

				qsort(varTable, numVars, sizeof(struct Var), compareVars);
				current += varLength;
			} else {
				fprintf(stderr, "Unable to create new variable \"%s\".\nDoes it use reserved words or already exist?", varName);
				free(varName);
				newToken = Token_throwError(1);
			}
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
		} else if ((newToken.data.var = searchVars(current, &current)) != NULL) {
			newToken.type = VAR;
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
