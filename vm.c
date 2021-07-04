#pragma once

#include "vm.h"
#include "kdebug.h"
#include "typeTable.h"
#include "kcompiler.h"
#include "byteArray.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "commonTypes.h"
#include "bytecode.h"

#include "opcodes.h"
#include "opcode_enums.h"
#include "opcode_impl.h"

void vm_error(VM* vm, const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm->ip - vm->chunk->code - 1;
	int line = linearr_get_line(&vm->chunk->lines, instruction);
	fprintf(stderr, "[line %d] in script\n", line);
	vm->stack.stacktop = vm->stack.stack;
}

void vm_init(VM* vm) {
	vm->stack.stacktop = vm->stack.stack;
	opcode_init_reg(&vm->opReg);
	opcode_impl_generate(&vm->opReg);
	typetbl_init(&vm->typeTable);
	typearr_init(&vm->stackTypes);
	//HashTable_Init(&vm->strings);
	//HashTable_Init(&vm->globals);
}

void vm_free(VM* vm) {
	opcode_free_reg(&vm->opReg);
	typetbl_free(&vm->typeTable);
	typearr_free(&vm->stackTypes);
	//HashTable_Free(&vm->strings);
	//HashTable_Free(&vm->globals);
}

void vm_push(VM* vm, Byte1* value, TYPE_ID type, int size) {
	typearr_add(&vm->stackTypes, type);
	bytestk_push_arr(&vm->stack, value, size);
}

void vm_pop(VM* vm, Byte1* out_value, TYPE_ID type, int size) {
	typearr_remove(&vm->stackTypes, type);
	bytestk_pop_arr(&vm->stack, &out_value, size);
}

TYPE_ID vm_peek_type(VM* vm) {
	return vm->stackTypes.types[vm->stackTypes.count - 1];
}

TYPE_ID vm_peek_typec(VM* vm, int depth) {
	return vm->stackTypes.types[vm->stackTypes.count - (1 + depth)];
}

Byte1 vm_read_ip(VM* vm) {
	return *vm->ip++;
}

static InterpretResult __run_code(OpCodeRegister* reg, VM* vm, OpCode op) {
	OpCodeFn* fn = (OpCodeFn*)opcode_get_op_func(reg, op);
	return (*fn)(vm);
}

InterpretResult vm_run(VM* vm) {
#ifdef DEBUG_TRACE_EXECUTION
	printf("\n== Stack Debug ==\n");
#endif

	while (true) {

#ifdef DEBUG_TRACE_EXECUTION

		printf("\t\t");
		Byte1* currentByte = vm->stack.stacktop;
		for (int i = 0; i < vm->stackTypes.count; i++) {
			TYPE_ID type = vm->stackTypes.types[i];
			printf("[");
			ktype_print(currentByte, type);
			printf("]");
			int typeSize = typetbl_get_info(&vm->typeTable, type)->size;
			printf("(%ub) / ", typeSize);
			currentByte += typeSize;
			type++;
		}
		printf("-> (%ub)", vm->stack.stacktop - vm->stack.stack);
		printf("\n");
		debug_disassemble_opcode(vm->chunk, vm->ip - vm->chunk->code);
#endif

		Byte1 instruction = vm_read_ip(vm);
		__run_code(&vm->opReg, vm, instruction);
		break;
	}
	return INTERPRET_OK;
}

InterpretResult vm_interpret(VM* vm, const char* source) {
	ByteCode c;
	bytec_init(&c);

	if (!kcom_compile(source, &c)) {
		bytec_free(&c);
		return INTERPRET_COMPILE_ERROR;
	}

	vm->chunk = &c;
	vm->ip = vm->chunk->code;

	InterpretResult result = vm_run(vm);

	bytec_free(&c);
	return result;
	//return run();
}