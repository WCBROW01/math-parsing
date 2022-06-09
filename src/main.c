#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define __USE_GNU
#include <math.h>

#include "tokenstack.h"
#include "vartable.h"
#include "lexing.h"
#include "parsing.h"
#include "eval.h"

#define DEFAULT_MAXLEN 64

int main(void) {
	int inputMax = DEFAULT_MAXLEN;
	bool debugmode = false;
	bool stackmode = false;

	char *input = malloc(inputMax * sizeof(char));
	if (input == NULL) return 1;

	VarTable *globalVars = VarTable_new();

	// Set up pi and e as constants
	Var *const_pi = VarTable_insert(globalVars, "pi");
	const_pi->data = M_PIl;
	const_pi->flags = VAR_INIT | VAR_CONST;

	Var *const_e = VarTable_insert(globalVars, "e");
	const_e->data = M_El;
	const_e->flags = VAR_INIT | VAR_CONST;

	// Seed random number generator with current time
	srandom(time(NULL));

	printf("Enter any expressions you would like to evaluate (type \"exit\" to quit):\n");

	for (;;) {
		char c = EOF;
		int numChars = 0;

		printf("> ");

		// Get user input character by character until a newline is encountered
		while ((c = getchar()) != '\n' && c != EOF) {
			input[numChars++] = c;

			// Realloc if string is not large enough
			if (numChars == inputMax) {
				inputMax += DEFAULT_MAXLEN;
				input = realloc(input, inputMax * sizeof(char));
			}
		}

		input[numChars] = '\0';
		char *beginning = input;
		while (*beginning == ' ') beginning++;
		if (strcmp(beginning, "exit") == 0) break;

		// If the only input is a newline, do nothing and give a new prompt.
		if (*beginning == '\0') continue;
		else if (strncmp(input, "debugmode", 9) == 0) {
			// Toggle debug mode
			debugmode ^= 1;
			printf("Debug mode: %s.\n", debugmode ? "On" : "Off");
			continue;
		} else if (strncmp(input, "stackmode", 9) == 0) {
			// Toggle stack mode
			stackmode ^= 1;
			printf("Stack mode: %s.\n", stackmode ? "On" : "Off");
			continue;
		}

		if (debugmode) printf("Provided input: %s\n", input);
		TokenStack lexerOutput = lexInput(beginning, globalVars);
		TokenStack parserOutput = {0};

		if (debugmode && TokenStack_peek(&lexerOutput).type != ERR) {
			printf("Lexer result: ");
			TokenStack_print(&lexerOutput);
		} else if (TokenStack_peek(&lexerOutput).type == ERR) goto destruct;

		if (!stackmode) parserOutput = parseTokens(&lexerOutput);

		if (debugmode && !stackmode) {
			printf("Parser result: ");
			TokenStack_print(&parserOutput);
		}

		if (stackmode) evaluateTokenStack(&lexerOutput);
		else evaluateTokenStack(&parserOutput);

		destruct:
		TokenStack_delete(&lexerOutput);
		TokenStack_delete(&parserOutput);
	}

	VarTable_delete(globalVars);
	free(input);
	return 0;
}
