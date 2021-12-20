#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opstack.h"
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
		if (strncmp(beginning, "exit", 4) == 0) exit(0);

		printf("Provided input: %s\n", input);
		OpStack parsedInput = parseInput(beginning);
		printf("Parsed result in RPN: ");
		OpStack_print(&parsedInput);

		long double answer = evaluateOpStack(&parsedInput);
		printf("Answer: %.15Lg\n", answer);

		OpStack_delete(&parsedInput);
	}

	free(input);
	return 0;
}
