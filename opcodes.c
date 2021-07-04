#include "opcodes.h"
#include "common.h"
#include "genericArray.h"

GenericFn* opcode_get_op_func(OpCodeRegister* reg, OpCode op) {
	OpCodeHolder* elements = reg->opcodes.elements;
	for (int i = 0; i < reg->opcodes.count; i++) {
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
