#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tokenstack.h"
#include "lexing.h"

static_assert(NUM_OPERATORS == 6, "Exhaustive handling of operators in ISOPERATOR");
#define ISOPERATOR(op) (op == '+' || op == '-' || op == '*' || op == '/' || op == '%' || op == '^')

static void pushOperator(TokenStack *outputStack, const char *operator) {
	static_assert(NUM_OPERATORS == 6, "Exhaustive handling of operators in pushOperator");
	Token newOperator = {.type = OPERATOR};

	switch(*operator) {
	case '+':
		newOperator.data.operator = ADD;
		break;
	case '-':
		newOperator.data.operator = SUB;
		break;
	case '*':
		newOperator.data.operator = MUL;
		break;
	case '/':
		newOperator.data.operator = DIV;
		break;
	case '%':
		newOperator.data.operator = MOD;
		break;
	case '^':
		newOperator.data.operator = EXP;
		break;
	default:
		fprintf(stderr, "Invalid operator '%c'.\n", *operator);
		newOperator = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newOperator);
}

static_assert(NUM_DELIMS == 3, "Exhaustive handling of delimiters in ISDELIM");
#define ISDELIM(delim) (delim == '(' || delim == ')' || delim == ',')

static void pushDelim(TokenStack *outputStack, const char *delim) {
	static_assert(NUM_DELIMS == 3, "Exhaustive handling of delimiters in pushDelim");
	Token newDelim = {.type = DELIM};

	switch(*delim) {
	case '(':
		newDelim.data.delim = OPEN_PAREN;
		break;
	case ')':
		newDelim.data.delim = CLOSE_PAREN;
		break;
	case ',':
		newDelim.data.delim = COMMA;
		break;
	default:
		fprintf(stderr, "Invalid delimiter '%c'.\n", *delim);
		newDelim = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newDelim);
}

static_assert(NUM_INTRINSICS == 15, "Exhaustive handling of intrinsics in ISINTRINSIC");
#define ISINTRINSIC(str) (strncmp(str, "abs(", 4) == 0 || strncmp(str, "sqrt(", 5) == 0 || strncmp(str, "cbrt(", 5) == 0 || strncmp(str, "ln(", 3) == 0 || strncmp(str, "sin(", 4) == 0 || strncmp(str, "cos(", 4) == 0 || strncmp(str, "tan(", 4) == 0 || strncmp(str, "asin(", 5) == 0 || strncmp(str, "acos(", 5) == 0 || strncmp(str, "atan2(", 6) == 0 || strncmp(str, "atan(", 5) == 0 || strncmp(str, "rand(", 5) == 0 || strncmp(str, "floor(", 6) == 0 || strncmp(str, "ceil(", 5) == 0 || strncmp(str, "ldexp(", 6) == 0)

static void pushIntrinsic(TokenStack *outputStack, char *str, char **endp) {
	static_assert(NUM_INTRINSICS == 15, "Exhaustive handling of intrinsics in pushIntrinsic");
	Token newIntrinsic = {.type = INTRINSIC};

	if (strncmp(str, "abs", 3) == 0) {
		*endp = str + 3;
		newIntrinsic.data.intrinsic = ABS;
	} else if (strncmp(str, "sqrt", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = SQRT;
	} else if (strncmp(str, "cbrt", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = CBRT;
	} else if (strncmp(str, "ln", 2) == 0) {
		*endp = str + 2;
		newIntrinsic.data.intrinsic = LN;
	} else if (strncmp(str, "sin", 3) == 0) {
		*endp = str + 3;
		newIntrinsic.data.intrinsic = SIN;
	} else if (strncmp(str, "cos", 3) == 0) {
		*endp = str + 3;
		newIntrinsic.data.intrinsic = COS;
	} else if (strncmp(str, "tan", 3) == 0) {
		*endp = str + 3;
		newIntrinsic.data.intrinsic = TAN;
	} else if (strncmp(str, "asin", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = ASIN;
	} else if (strncmp(str, "acos", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = ACOS;
	} else if (strncmp(str, "atan2", 5) == 0) {
		*endp = str + 5;
		newIntrinsic.data.intrinsic = ATAN2;
	} else if (strncmp(str, "atan", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = ATAN;
	} else if (strncmp(str, "rand", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = RAND;
	} else if (strncmp(str, "floor", 5) == 0) {
		*endp = str + 5;
		newIntrinsic.data.intrinsic = FLOOR;
	} else if (strncmp(str, "ceil", 4) == 0) {
		*endp = str + 4;
		newIntrinsic.data.intrinsic = CEIL;
	} else if (strncmp(str, "ldexp", 5) == 0) {
		*endp = str + 5;
		newIntrinsic.data.intrinsic = LDEXP;
	} else {
		fprintf(stderr, "Invalid intrinsic encountered while lexing.\n");
		newIntrinsic = Token_throwError(2);
	}

	TokenStack_push(outputStack, &newIntrinsic);
}

/* There is barely any benefit to doing anything other than substitution,
 * so we just won't. */
#define ISCONSTANT(str) (strncmp(str, "pi", 2) == 0 || *str == 'e')

static void pushOperand(TokenStack *outputStack, Operand_t operand) {
	Token temp = {
		.type = OPERAND,
		.data.operand = operand
	};

	TokenStack_push(outputStack, &temp);
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

		if (isdigit(*current)) {
			pushOperand(&outputStack, strtold(current, &current));
		} else if (*current == ' ') {
			current++;
		} else if (*current == '+' || *current == '-') {
			// Checks if the + or - is an operator or part of an operand
			if (lastToken.type == OPERATOR || lastToken.type == NULL_TOKEN || (lastToken.type == DELIM && lastToken.data.delim == OPEN_PAREN)) {
				pushOperand(&outputStack, strtold(current, &current));
			} else {
				pushOperator(&outputStack, current++);
			}
		} else if (ISOPERATOR(*current)) {
			pushOperator(&outputStack, current++);
		} else if (ISDELIM(*current)) {
			pushDelim(&outputStack, current++);
		} else if (ISINTRINSIC(current)) {
			pushIntrinsic(&outputStack, current, &current);
		} else if (ISCONSTANT(current)) {
			if (strncmp(current, "pi", 2) == 0) {
				pushOperand(&outputStack, M_PI);
				current += 2;
			} else if (*current++ == 'e') pushOperand(&outputStack, M_PI);
		} else {
			// Invalid token while lexing.
			fprintf(stderr, "Invalid input provided.\n");
			Token error = Token_throwError(2);
			TokenStack_push(&outputStack, &error);
		}

		lastToken = TokenStack_peek(&outputStack);
	}

	return outputStack;
}
