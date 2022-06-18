# math-parsing

A calculator program that is a playground for me to play with different methods of lexing and parsing. The main program is a REPL that lets you enter in mathematical expressions, with a few different commands that function as switches. Both standard infix notation and [Reverse Polish notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) are supported.

The program is built in pure C, with the only dependency being libc, but I might end up replacing parts of the program that use it over time as standard C functions become inadequate or require me to do weird things.

## Building the program

Since there are no dependencies, building is very easy! All you have to do is the standard makefile build process and you'll get a `calculator` executable as a result.

```sh
./configure
make
```

## What is included

### Arithmetic operators

Addition, subtraction, multiplication, division, modulus, and exponentiation are all supported. Parentheses are supported and the order of operations is honored, so anything that works in a basic calculator should work properly. Expressions like 2(1) and 2sqrt(2) are supported and will imply multiplication exactly as expected.

### Comparison operators

All standard comparison operators seen in most programming languages are supported. There are operators for equality, inequality, less than, greater than, less or equal, and greater or equal. A comparison will return 0 if false and 1 if true. A proper boolean type may be added later.

### Mathematical constants

*e* and *pi* will expand into their respective constants in the expression.

### Variables

You are able to declare variables using the let keyword, assign and reassign values to them using ‘=’, and use them within in any mathematical expression. You may not name them the same as any built-in functions or include any operators or delimiters within the name. You can also define constants using the const keyword.

### Built-in functions

| Function      | Output                                                       |
| ------------- | ------------------------------------------------------------ |
| abs(x)        | The absolute value of x                                      |
| sqrt(x)       | The square root of x                                         |
| cbrt(x)       | The cube root of x                                           |
| ln(x)         | The natural logarithm (log<sub>e</sub>) of x                 |
| log10(x)      | The base 10 logarithm of x                                   |
| log2(x)       | The base 2 logarithm of x                                    |
| sin(x)        | The sine of x                                                |
| cos(x)        | The cosine of x                                              |
| tan(x)        | The tangent of x                                             |
| asin(x)       | The arcsine of x                                             |
| acos(x)       | The arc-cosine of x                                          |
| atan(x)       | The arctangent of x                                          |
| atan2(y, x)   | The angle in radians between the positive x axis and the point (x, y) |
| rand(x)       | A random floating-point number between 0 and x.              |
| floor(x)      | The result of rounding down x                                |
| ceil(x)       | The result of rounding up x                                  |
| ldexp(x, exp) | The result of the expression x ⋅ 2<sup>exp</sup>             |
| min(x, y)     | The minimum value between x and y                            |
| max(x, y)     | The maximum value between x and y                            |

### Switches

You can switch certain features on and off by providing certain commands.

| Command   | Result                                                       |
| --------- | ------------------------------------------------------------ |
| debugmode | Toggles debug mode. This will display the output of the lexing and parsing process. |
| stackmode | Toggles stack mode. This will allow you to input an expression in Reverse Polish notation. |



## Using Reverse Polish notation

You can switch between infix and Reverse Polish notation using the stackmode command. In this mode, the infix parsing stage of the program is skipped entirely and whatever you type will be provided directly to the stack machine that evaluates expressions. Each part of the expression is separated by a space or comma. Including parentheses does absolutely nothing, and any present will be skipped. This is allowed purely in case it makes expressions easier to read. Currently, providing expressions with extra operators or operands is undefined behavior.

### Arithmetic operations

Every arithmetic operator is supported. Each one will take two arguments in the following order: *a b op*
which will evaluate the equivalent expression *a op b* and push the result back onto the stack.
Example: *1 2 +* evaluates *1 + 2* and pushes 3 onto the stack.

### Functions

Functions are evaluated in the same manner, with the parameters of the function being evaluated from left to right, so a function would be invoked as follows: *a b ... func*
Example: *2 sqrt* evaluates *sqrt(2)* and pushes √2 onto the stack.
