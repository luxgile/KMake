#pragma once

#include "vm.h"

typedef enum
{
	OP_CONSTANT,
	OP_DEFINE_GLOBAL,
	OP_POP,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULT,
	OP_DIVIDE,
	OP_TRUE,
	OP_FALSE,
	OP_EQUALS,
	OP_NOT,
	OP_GREAT,
	OP_LESS,
	OP_RETURN,
	OP_PRINT,
} OpCode;

typedef void (*OpCodeFn) (VM* vm);

typedef struct {
	OpCode op;
	OpCodeFn fn;
} OpCodeHolder;