#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "arena.h"
#include "tokenstack.h"
#include "vartable.h"
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

static int searchTable(char *str, const char *table[], int length, char **endptr) {
	bool found = false;
	int index;

	for (index = 0; index < length; index++) {
		if (substreq(str, table[index], endptr)) {
			found = true;
			break;
		}
	}

	return found ? index : -1;
}

static Var *searchVars(VarTable *table, char *str, char **endptr) {
	size_t length = 0;
	while (str[length] != ' ' && str[length] != '\0' &&
		   searchTable(str + length,  OPERATOR_STR_TABLE, NUM_OPERATORS,  NULL) == -1 &&
		   searchTable(str + length,     DELIM_STR_TABLE, NUM_DELIMS,     NULL) == -1
	) length++;

	// Temporarily copy the variable into a separate string.
	char temp[length + 1];
	strncpy(temp, str, length + 1);
	temp[length] = '\0';

	struct Var *result = VarTable_search(table, temp);
	if (endptr != NULL && result != NULL) *endptr = str + length;
	return result;
}

static bool verifyVar(VarTable *table, char *var) {
	if (var == NULL) return false;
	if (VarTable_search(table, var) != NULL) return false;

	// Check if the variable name matches any intrinsics
	for (size_t i = 0; i < NUM_INTRINSICS; i++) {
		if (strcmp(var, INTRINSIC_STR_TABLE[i]) == 0) return false;
	}

	// Check for any reserved characters in string (this looks stupid I hate it)
	while (*var != '\0') {
		if (searchTable(var, OPERATOR_STR_TABLE, NUM_OPERATORS, NULL) != -1 ||
			searchTable(var,    DELIM_STR_TABLE, NUM_DELIMS,    NULL) != -1
		) return false;
		else var++;
	}

	return true;
}

static Token createVar(VarTable *table, char *str, char **endptr) {
	size_t varLength;
	Arena *tableArena = VarTable_getArena(table);

	while (*str == ' ') str++; // Skip spaces
	for (varLength = 1; str[varLength] != ' '; varLength++);
	char varName[varLength + 1];
	strncpy(varName, str, varLength);
	varName[varLength] = '\0';

	if (verifyVar(table, varName)) {
		char *entry = Arena_alloc(tableArena, varLength + 1);
		if (!entry) {
			return Token_throwError(ERR_FAILED_VAR_ALLOC, varName);
		}

		memcpy(entry, varName, varLength + 1);
		Var *newVar = VarTable_insert(table, entry);
		Token newToken = {
			.type = VAR,
			.data.var = newVar
		};

		*endptr += varLength;
		return newToken;
	} else {
		return Token_throwError(ERR_INVALID_VAR, varName);
	}
}

TokenStack lexInput(char *input, VarTable *globalVars) {
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

		if (isdigit(*current) || *current == '.') {
			// This condition creates an infinite loop if not caught.
			if (*current == '.' && !isdigit(current[1])) {
				newToken = Token_throwError(ERR_INVALID_INPUT);
			} else {
				newToken = (Token){
					.type = OPERAND,
					.data.operand = strtold(current, &current)
				};
			}
		} else if (*current == ' ') {
			current++;
			continue;
		} else if (substreq(current, "let ", &current)) {
			newToken = createVar(globalVars, current, &current);
		} else if (substreq(current, "const ", &current)) {
			newToken = createVar(globalVars, current, &current);
			newToken.data.var->flags |= VAR_CONST;
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
		} else if ((newToken.data.operator = searchTable(current, OPERATOR_STR_TABLE, NUM_OPERATORS, &current)) != -1) {
			newToken.type = OPERATOR;
			if (newToken.data.operator == ASSIGN) {
				if (!variableAssigned) {
					variableAssigned = true;
					current++;
				} else {
					newToken = Token_throwError(ERR_NOT_IMPLEMENTED, "Attempted to assign multiple variables in one statement");
				}
			}
		} else if ((newToken.data.delim = searchTable(current, DELIM_STR_TABLE, NUM_DELIMS, &current)) != -1) {
			newToken.type = DELIM;
		} else if ((newToken.data.var = searchVars(globalVars, current, &current)) != NULL) {
			newToken.type = VAR;
		} else if ((newToken.data.intrinsic = searchTable(current, INTRINSIC_STR_TABLE, NUM_INTRINSICS, &current)) != -1) {
			newToken.type = INTRINSIC;
#pragma GCC diagnostic pop
		} else {
			// Invalid token while lexing.
			newToken = Token_throwError(ERR_INVALID_INPUT);
		}

		TokenStack_push(&outputStack, newToken);
		lastToken = newToken;
	}

	return outputStack;
}
