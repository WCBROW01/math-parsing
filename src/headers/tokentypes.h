#ifndef TOKENTYPES_H
#define TOKENTYPES_H

typedef long double Operand_t;
typedef int Err_t;

enum Operator {
	ADD,
	SUB,
	MUL,
	DIV,
	EXP,
	NUM_OPERATORS
};

enum Delim {
	OPEN_PAREN,
	CLOSE_PAREN,
	NUM_DELIMS
};

enum Intrinsic {
	ABS,
	SQRT,
	LN,
	SIN,
	COS,
	TAN,
	ARCSIN,
	ARCCOS,
	ARCTAN,
	NUM_INTRINSICS
};

enum TokenType {
	OPERAND,
	OPERATOR,
	DELIM,
	INTRINSIC,
	ERR,
	NULL_TOKEN,
	NUM_TYPES
};

union TokenData {
	Operand_t operand;
	enum Operator operator;
	enum Delim delim;
	enum Intrinsic intrinsic;
	Err_t err;
};

#endif
