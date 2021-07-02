#include "opcodes.h"
#include "common.h"
#include "genericArray.h"

static OpCodeFn* __get_op_func(OpCodeRegister* reg, OpCode op) {
	OpCodeHolder* elements = reg->opcodes.elements;
	for (size_t i = 0; i < reg->opcodes.count; i++) {
		if(elements[i].op == op) return &elements[i].fn;
	}
	return NULL;
}

void opcode_init_reg(OpCodeRegister* reg) {
	genarr_init(&reg->opcodes, sizeof(OpCodeHolder));
}

void opcode_free_reg(OpCodeRegister* reg) {
	genarr_free(&reg->opcodes);
}

void opcode_register_code(OpCodeRegister* reg, OpCodeHolder code) {
	genarr_add(&reg->opcodes, &code);
}

InterpretResult opcode_run_code(OpCodeRegister* reg, VM* vm, OpCode op) {
	OpCodeFn* fn = __get_op_func(reg, op);
	return (*fn)(vm);
}
