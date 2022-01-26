#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenstack.h"
#include "lexing.h"
#include "parsing.h"
#include "eval.h"

#define DEFAULT_MAXLEN 64

int main(void) {
	int inputMax = DEFAULT_MAXLEN;
	bool debug = false;
	char *input = malloc(inputMax * sizeof(char));
	if (input == NULL) return 1;

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
			debug ^= 1;
			printf("Debug mode: %s.\n", debug ? "On" : "Off");
			continue;
		}

		printf("Provided input: %s\n", input);
		TokenStack lexerOutput = lexInput(beginning);

		if (debug && TokenStack_peek(&lexerOutput).type != ERR) {
			printf("Lexer result: ");
			TokenStack_print(&lexerOutput);
		}

		TokenStack parserOutput = parseTokens(&lexerOutput);

		if (debug) {
			printf("Parser result: ");
			TokenStack_print(&parserOutput);
		}

		long double answer = evaluateTokenStack(&parserOutput);
		printf("%.15Lg\n", answer);

		TokenStack_delete(&lexerOutput);
		TokenStack_delete(&parserOutput);
	}

	free(input);
	return 0;
}
