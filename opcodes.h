#pragma once

#include "vm.h"
#include "genericArray.h"

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

typedef InterpretResult(*OpCodeFn) (VM* vm);

typedef struct {
	OpCode op;
	OpCodeFn fn;
} OpCodeHolder;

typedef struct {
	GenericArray opcodes;
} OpCodeRegister;

void opcode_init_reg(OpCodeRegister* reg);
void opcode_free_reg(OpCodeRegister* reg);

void opcode_register_code(OpCodeRegister* reg, OpCodeHolder code);
void opcode_run_code(OpCodeRegister* reg, VM* vm, OpCode op);