#include <stdio.h>
#include <stdlib.h>
#include "opstack.h"
#include "parsing.h"
#include "eval.h"

#define DEFAULT_MAXLEN 64

int main(void) {
	int inputMax = DEFAULT_MAXLEN;
	char *input = malloc(inputMax * sizeof(char));
	if (input == NULL) return 1;

	printf("Enter the expression you would like to evaluate: ");

	char c = EOF;
	int numChars = 0;

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

	printf("Provided input: %s\n", input);
	OpStack parsedInput = parseInput(input);
	printf("Parsed result in RPN: ");
	OpStack_print(&parsedInput);

	long double answer = evaluateOpStack(&parsedInput);
	printf("Answer: %.18Lg\n", answer);

	OpStack_delete(&parsedInput);
	free(input);
	return 0;
}
