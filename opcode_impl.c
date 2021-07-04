#include "opcode_impl.h"
#include "common.h"
#include "opcodes.h"
#include "commonTypes.h"
#include <stdio.h>
#include "vm.h"

#define GET_TYPE_SIZE(vm, type) (typetbl_get_info(&vm->chunk->typeTable, type)->size)

//case OP_DEFINE_GLOBAL:
//{
//	StringPointer* name = bytearr_read(&vm.chunk->constants, StringPointer*, READ_BYTE());
//	TYPE_ID type = READ_BYTE();
//	uint8_t varId = READ_BYTE();
//	HashTable_Set(&vm.globals, name, &varId, TYPEID_BOOL);
//	break;
//}

static InterpretResult __op_constant(VM* vm) {
	TYPE_ID type = vm_read_ip(vm);
	Byte1* bytes = bytearr_read(&vm->chunk->constants, vm_read_ip(vm));
	vm_push(vm, bytes, type, GET_TYPE_SIZE(vm, type));
	return INTERPRET_NONE;
}

static InterpretResult __op_negate(VM* vm) {
	double b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	b = -b;
	vm_push(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	return INTERPRET_NONE;
}

static InterpretResult __op_not(VM* vm) {
	bool b;
	vm_pop(vm, (Byte1*)&b, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	b = !b;
	vm_push(vm, (Byte1*)&b, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_divide(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	double result = a / b;
	vm_push(vm, (Byte1*)&result, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	return INTERPRET_NONE;
}

static InterpretResult __op_mult(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	double result = a * b;
	vm_push(vm, (Byte1*)&result, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	return INTERPRET_NONE;
}

static InterpretResult __op_subtract(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	double result = a - b;
	vm_push(vm, (Byte1*)&result, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	return INTERPRET_NONE;
}

static InterpretResult __op_add(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	double result = a + b;
	vm_push(vm, (Byte1*)&result, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	return INTERPRET_NONE;
}

static InterpretResult __op_equals(VM* vm) {
	TYPE_ID type = vm_peek_type(vm);
#ifdef DEBUG_SAFETY
	if (type != vm_peek_typec(vm, 1)) {
		printf("Different types cannot be compared.");
		return INTERPRET_RUNTIME_ERROR;
	}
#endif
	Byte1* a = NULL;
	Byte1* b = NULL;
	vm_pop(vm, b, type, GET_TYPE_SIZE(vm, type));
	vm_pop(vm, a, type, GET_TYPE_SIZE(vm, type));
	bool result = ktype_equality(a, b, type);
	vm_push(vm, (Byte1*)&result, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_great(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	bool result = a > b;
	vm_push(vm, (Byte1*)&result, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_less(VM* vm) {
	double a, b;
	vm_pop(vm, (Byte1*)&b, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	vm_pop(vm, (Byte1*)&a, TYPEID_DEC, GET_TYPE_SIZE(vm, TYPEID_DEC));
	bool result = a < b;
	vm_push(vm, (Byte1*)&result, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_false(VM* vm) {
	bool b = false;
	vm_push(vm, &b, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_true(VM* vm) {
	bool b = true;
	vm_push(vm, &b, TYPEID_BOOL, GET_TYPE_SIZE(vm, TYPEID_BOOL));
	return INTERPRET_NONE;
}

static InterpretResult __op_pop(VM* vm) {
	TYPE_ID type = vm_peek_type(vm);
	vm_pop(vm, NULL, type, GET_TYPE_SIZE(vm, type));
	return INTERPRET_NONE;
}

static InterpretResult __op_print(VM* vm) {
	TYPE_ID type = vm_peek_type(vm);
	Byte1* byte = NULL;
	vm_pop(vm, byte, type, GET_TYPE_SIZE(vm, type));
	ktype_print(byte, type);
	return INTERPRET_NONE;
}

static InterpretResult __op_return(VM* vm) {
	return INTERPRET_OK;
}

static OpCodeHolder __create_holder(OpCode code, OpCodeFn fn) {
	OpCodeHolder holder;
	holder.op = code;
	holder.fn = fn;
	return holder;
}

void opcode_impl_generate(OpCodeRegister* reg) {
	opcode_register_code(reg, __create_holder(OP_CONSTANT, __op_constant));
	opcode_register_code(reg, __create_holder(OP_NEGATE, __op_negate));
	opcode_register_code(reg, __create_holder(OP_NOT, __op_not));
	opcode_register_code(reg, __create_holder(OP_DIVIDE, __op_divide));
	opcode_register_code(reg, __create_holder(OP_MULT, __op_mult));
	opcode_register_code(reg, __create_holder(OP_SUBTRACT, __op_subtract));
	opcode_register_code(reg, __create_holder(OP_ADD, __op_add));
	opcode_register_code(reg, __create_holder(OP_EQUALS, __op_equals));
	opcode_register_code(reg, __create_holder(OP_GREAT, __op_great));
	opcode_register_code(reg, __create_holder(OP_LESS, __op_less));
	opcode_register_code(reg, __create_holder(OP_FALSE, __op_false));
	opcode_register_code(reg, __create_holder(OP_TRUE, __op_true));
	opcode_register_code(reg, __create_holder(OP_FALSE, __op_pop));
	opcode_register_code(reg, __create_holder(OP_PRINT, __op_print));
	opcode_register_code(reg, __create_holder(OP_RETURN, __op_return));
}
