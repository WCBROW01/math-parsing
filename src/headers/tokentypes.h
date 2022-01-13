#ifndef TOKENTYPES_H
#define TOKENTYPES_H

typedef long double Operand_t;
typedef int Err_t;

enum TokenType {
	Operand,
	Operator,
	Delim,
	Err,
	Null,
	NumTypes
};

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


#endif
