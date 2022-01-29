#ifndef TOKENSTACK_H
#define TOKENSTACK_H

#define DEFAULT_LENGTH 64

// Struct for operators and operands (with a boolean to differentiate)
typedef struct Token {
	enum TokenType type;
	union TokenData data;
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

extern Token Token_throwError(int errorlevel);

#endif
