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

void vm_error(VM* vm, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm->ip - vm->chunk->code - 1;
	int line = linearr_get_line(&vm->chunk->lines, instruction);
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

void vm_init(VM* vm)
{
	resetStack();

	typearr_init(&vm->stackTypes);
	HashTable_Init(&vm->strings);
	HashTable_Init(&vm->globals);
}

void vm_free(VM* vm)
{
	typearr_free(&vm->stackTypes);
	HashTable_Free(&vm->strings);
	HashTable_Free(&vm->globals);
}

void vm_push_dec(VM* vm, double value)
{
	typearr_add(&vm->stackTypes, TYPEID_DEC);
	BYTESTK_PUSH(&vm->stack, double, value);
}

void vm_push_bool(VM* vm, bool value)
{
	typearr_add(&vm->stackTypes, TYPEID_BOOL);
	BYTESTK_PUSH(&vm->stack, bool, value);
}

void vm_pop_dec(VM* vm, double* out_value)
{
	typearr_remove(&vm->stackTypes, TYPEID_DEC);
	BYTESTK_POP(&vm->stack, double, out_value);
}

void vm_pop_bool(VM* vm, bool* out_value)
{
	typearr_remove(&vm->stackTypes, TYPEID_BOOL);
	BYTESTK_POP(&vm->stack, bool, out_value);
}

TYPE_ID vm_peek_type(VM* vm)
{
	return vm->stackTypes.types[vm->stackTypes.count - 1];
}

InterpretResult vm_interpret(VM* vm, const char* source)
{
	ByteCode c;
	bytec_init_chunk(&c);

	if (!kcom_compile(source, &c))
	{
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

inline Byte1* vm_read_ip(VM* vm)
{
	return *vm->ip++;
}

InterpretResult vm_run(VM* vm)
{
#define READ_BYTE() (*vm->ip++)
#define BINARY_OP(op, intype, inid, outtype, outid) \
    do { \
      intype b = CAST(vm_pop(inid), intype); \
      intype a = CAST(vm_pop(inid), intype); \
	  outtype result = a op b; \
      vm_push(&result, outid); \
    } while (false)

#ifdef DEBUG_TRACE_EXECUTION
	printf("\n== Stack Debug ==\n");
#endif

	while(true)
	{

#ifdef DEBUG_TRACE_EXECUTION

		printf("\t\t");
		Byte1* currentByte = &vm->stack;
		int stackSize;
		for (int i = 0; i < vm->stackTypes.count; i++)
		{
			TYPE_ID type = vm->stackTypes.types[i];
			printf("[");
			switch (type)
			{
			case TYPEID_VOID: printf("void"); break;
			case TYPEID_DEC: printf("%g", CAST(currentByte, double)); break;
			case TYPEID_BOOL: printf("%s", CAST(currentByte, bool) ? "true" : "false"); break;
			case TYPEID_STRING: printf("%s", (char*)CAST(currentByte, StringPointer*)->base.p); break;
			}
			printf("]");
			int typeSize = typetbl_get_info(type)->size;
			printf("(%ub) / ", typeSize);
			currentByte += typeSize;
			type++;
		}
		printf("-> (%ub)", vm->stackTop - vm->stack);
		printf("\n");
		debug_disassemble_opcode(vm->chunk, vm->ip - vm->chunk->code);
#endif

		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{

		case OP_CONSTANT:
		{
			TYPE_ID type = READ_BYTE();
			Byte1* bytes = ByteArray_ReadByte(&vm.chunk->constants, READ_BYTE());
			vm_push(bytes, type);
			break;
		}

		case OP_DEFINE_GLOBAL:
		{
			StringPointer* name = bytearr_read(&vm.chunk->constants, StringPointer*, READ_BYTE());
			TYPE_ID type = READ_BYTE();
			uint8_t varId = READ_BYTE();
			HashTable_Set(&vm.globals, name, &varId, TYPEID_BOOL);
			break;
		}

		case OP_NEGATE:
		{
			double d = -CAST(vm_pop(TYPEID_DEC), double);
			vm_push(&d, TYPEID_DEC);
			break;
		}

		case OP_NOT:
		{
			bool b = !CAST(vm_pop(TYPEID_BOOL), bool);
			vm_push(&b, TYPEID_BOOL);
			break;
		}

		case OP_ADD:		BINARY_OP(+, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_SUBTRACT:	BINARY_OP(-, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_MULT:		BINARY_OP(*, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_DIVIDE:		BINARY_OP(/ , double, TYPEID_DEC, double, TYPEID_DEC); break;

		case OP_EQUALS: { bool result = Equality(); vm_push(&result, TYPEID_BOOL); break; }
		case OP_GREAT:		BINARY_OP(> , double, TYPEID_DEC, bool, TYPEID_BOOL); break;
		case OP_LESS:		BINARY_OP(< , double, TYPEID_DEC, bool, TYPEID_BOOL); break;

		case OP_TRUE: { bool b = true; vm_push(&b, TYPEID_BOOL); break; }
		case OP_FALSE: { bool b = false; vm_push(&b, TYPEID_BOOL); break; }

					 //case OP_IS:			BINARY_OP(< , bool, TYPEID_BOOL, bool, TYPEID_BOOL); break;

		case OP_PRINT:
		{
			TYPE_ID type = vm_peek_type();
			ktype_print(vm_pop(type), type);
			printf("\n");
			break;
		}

		case OP_POP: vm_pop(vm_peek_type()); break;
		case OP_RETURN: return INTERPRET_OK;

		}
	}

#undef READ_BYTE
#undef BINARY_OP
}