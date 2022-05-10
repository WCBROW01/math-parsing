#ifndef TOKENSTACK_H
#define TOKENSTACK_H

#define DEFAULT_LENGTH 64

#define VAR_INIT 0b01
#define VAR_CONST 0b10

// Types of tokens
typedef long double Operand_t;

enum Operator {
	ASSIGN,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	EXP,
	NUM_OPERATORS
};

extern const char *OPERATOR_STR_TABLE[NUM_OPERATORS];

enum Delim {
	OPEN_PAREN,
	CLOSE_PAREN,
	COMMA,
	NUM_DELIMS
};

extern const char *DELIM_STR_TABLE[NUM_DELIMS];

enum Intrinsic {
	ABS,
	SQRT,
	CBRT,
	LN,
	LOG10,
	SIN,
	COS,
	TAN,
	ASIN,
	ACOS,
	ATAN,
	ATAN2,
	RAND,
	FLOOR,
	CEIL,
	LDEXP,
	MIN,
	MAX,
	NUM_INTRINSICS
};

extern const char *INTRINSIC_STR_TABLE[NUM_INTRINSICS];

// Type for variables, this should rarely be stored outside of a var table.
typedef struct Var {
	char *name;
	unsigned char flags;
	Operand_t data;
} Var;

enum Err {
	ERR_INVALID_VAR,
	ERR_INVALID_INPUT,
	ERR_INVALID_TOKEN,
	ERR_REASSIGNED_CONSTANT,
	ERR_FAILED_VAR_ALLOC
};

enum TokenType {
	OPERAND,
	OPERATOR,
	DELIM,
	INTRINSIC,
	VAR,
	ERR,
	NULL_TOKEN,
	NUM_TYPES
};

union TokenData {
	Operand_t operand;
	enum Operator operator;
	enum Delim delim;
	enum Intrinsic intrinsic;
	struct Var *var;
	enum Err err;
};

// Struct for tokens, with an enum to denote type and a union containing the token itself.
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

extern Token Token_throwError(enum Err errorlevel);

#endif
