#ifndef TOKENTYPES_H
#define TOKENTYPES_H

typedef long double Operand_t;
typedef int Err_t;

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

struct Var {
	char *name;
	short index;
	Operand_t data;
};

// This is like the most global global variable ever, sue me.
#define VAR_CAP 1024
extern short numVars;
extern struct Var varTable[VAR_CAP];

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
	Err_t err;
};

#endif
