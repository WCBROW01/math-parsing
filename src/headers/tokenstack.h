#ifndef TOKENSTACK_H
#define TOKENSTACK_H

#include "tokentypes.h"

#define DEFAULT_LENGTH 64

// Struct for operators and operands (with a boolean to differentiate)
typedef struct Token {
	enum TokenType type;
	void *data;
} Token;

typedef struct TokenStack {
	int length;
	Token *top;
	Token *tokens;
} TokenStack;

extern TokenStack TokenStack_new();
extern void TokenStack_delete(TokenStack *stack);

extern void TokenStack_push(TokenStack *stack, const Token *data);
extern Token TokenStack_pop(TokenStack *stack);
extern Token TokenStack_peek(const TokenStack *stack);

extern void TokenStack_print(const TokenStack *stack);

extern Token Token_new(enum TokenType type);
extern void Token_delete(Token *token);
extern Token Token_throwError(int errorlevel);

#endif
