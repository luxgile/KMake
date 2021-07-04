#ifndef __OPCODES_HEADER__
#define __OPCODES_HEADER__

#include "common.h"
#include "opcode_enums.h"
#include "genericArray.h"

typedef struct OpCodeHolder {
	OpCode op;
	GenericFn fn;
} OpCodeHolder;

typedef struct OpCodeRegister {
	GenericArray opcodes;
} OpCodeRegister;

void opcode_init_reg(OpCodeRegister* reg);
void opcode_free_reg(OpCodeRegister* reg);

void opcode_register_code(OpCodeRegister* reg, OpCodeHolder code);
GenericFn* opcode_get_op_func(OpCodeRegister* reg, OpCode op);

#endif