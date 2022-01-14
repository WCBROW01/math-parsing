#ifndef TOKENTYPES_H
#define TOKENTYPES_H

typedef long double Operand_t;
typedef int Err_t;

enum Operator {
	Add,
	Sub,
	Mul,
	Div,
	Exp,
	NumOperators
};

enum Delim {
	OpenParen,
	CloseParen,
	NumDelims
};

enum TokenType {
	Operand,
	Operator,
	Delim,
	Err,
	Null,
	NumTypes
};

union TokenData {
	Operand_t operand;
	enum Operator operator;
	enum Delim delim;
	Err_t err;
};

#endif
