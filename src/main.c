#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenstack.h"
#include "parsing.h"
#include "eval.h"

#define DEFAULT_MAXLEN 64

int main(void) {
	int inputMax = DEFAULT_MAXLEN;
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

		printf("Provided input: %s\n", input);
		TokenStack parsedInput = parseInput(beginning);
		printf("Parsed result in RPN: ");
		TokenStack_print(&parsedInput);

		long double answer = evaluateOpStack(&parsedInput);
		printf("Answer: %.15Lg\n", answer);

		TokenStack_delete(&parsedInput);
	}

	free(input);
	return 0;
}
