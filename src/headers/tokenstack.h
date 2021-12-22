#ifndef TOKENSTACK_H
#define TOKENSTACK_H

#define MAX_LENGTH 64

typedef long double Token_t;

typedef enum Operator {
	Err = -1,
	None,
	Add,
	Sub,
	Mul,
	Div,
	Exp,
	OpenParen,
	CloseParen
} Operator;

// Struct for operators and operands (with a boolean to differentiate)
typedef struct Token {
	Operator op;
	Token_t data;
} Token;

typedef struct TokenStack {
	int length;
	int top;
	Token *tokens;
} TokenStack;

extern TokenStack TokenStack_new();
extern void TokenStack_delete(TokenStack *stack);

extern void TokenStack_push(TokenStack *stack, const Token *data);
extern Token TokenStack_pop(TokenStack *stack);
extern Token TokenStack_peek(const TokenStack *stack);

extern char operatorToChar(const Token *operator);
extern void TokenStack_print(const TokenStack *stack);

extern Token Token_throwError(int errorlevel);

#endif
