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

static void resetStack()
{
	vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = getLine(&vm.chunk->lines, instruction);
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

static void printValue(Byte1* bytes, TYPE_ID type)
{
	switch (type)
	{
	case TYPEID_DEC: printf("%g", CAST(bytes, double)); break;
	case TYPEID_BOOL: printf("%s", CAST(bytes, bool) ? "true" : "false"); break;
	case TYPEID_STRING: printf("%s", (char*)CAST(bytes, StringPointer*)->base.p); break;
	}
}

void initVM()
{
	resetStack();

	TypeArray_Init(&vm.stackTypes);
	HashTable_Init(&vm.strings);
	HashTable_Init(&vm.globals);
}

void freeVM()
{
	TypeArray_Free(&vm.stackTypes);
	HashTable_Free(&vm.strings);
	HashTable_Free(&vm.globals);
}

void VM_Push(Byte1* bytes, TYPE_ID type)
{
	TypeArray_AddType(&vm.stackTypes, type);

	int size = TypeTable_GetTypeInfo(type)->size;
	memcpy(vm.stackTop, bytes, size);
	vm.stackTop += size;
}

Byte1* VM_Pop(TYPE_ID type)
{
	if (type == TYPEID_VOID) return NULL;

	TypeArray_RemoveType(&vm.stackTypes, type);

	vm.stackTop -= TypeTable_GetTypeInfo(type)->size;
	return vm.stackTop;
}
Byte1* VM_PopPointer(TYPE_ID type)
{
	TypeArray_RemoveType(&vm.stackTypes, type);

	vm.stackTop -= sizeof(void*);
	return vm.stackTop;
}

static TYPE_ID peekType()
{
	return vm.stackTypes.types[vm.stackTypes.count - 1];
}

InterpretResult interpret(const char* source)
{
	ByteCode c;
	bytec_init_chunk(&c);

	if (!kcom_compile(source, &c))
	{
		bytec_free(&c);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &c;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	bytec_free(&c);
	return result;
	//return run();
}

static bool Equality()
{
	switch (peekType())
	{
	case TYPEID_BOOL: return CAST(VM_Pop(TYPEID_BOOL), bool) == CAST(VM_Pop(TYPEID_BOOL), bool);
	case TYPEID_DEC: return CAST(VM_Pop(TYPEID_DEC), double) == CAST(VM_Pop(TYPEID_DEC), double);
	case TYPEID_STRING: return CAST(VM_Pop(TYPEID_STRING), StringPointer*) == CAST(VM_Pop(TYPEID_STRING), StringPointer*);
	}

	return false;
}

InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define BINARY_OP(op, intype, inid, outtype, outid) \
    do { \
      intype b = CAST(VM_Pop(inid), intype); \
      intype a = CAST(VM_Pop(inid), intype); \
	  outtype result = a op b; \
      VM_Push(&result, outid); \
    } while (false)

#ifdef DEBUG_TRACE_EXECUTION
	printf("\n== Stack Debug ==\n");
#endif

	for (;;)
	{

#ifdef DEBUG_TRACE_EXECUTION

		printf("\t\t");
		Byte1* currentByte = &vm.stack;
		int stackSize;
		for (int i = 0; i < vm.stackTypes.count; i++)
		{
			TYPE_ID type = vm.stackTypes.types[i];
			printf("[");
			switch (type)
			{
			case TYPEID_VOID: printf("void"); break;
			case TYPEID_DEC: printf("%g", CAST(currentByte, double)); break;
			case TYPEID_BOOL: printf("%s", CAST(currentByte, bool) ? "true" : "false"); break;
			case TYPEID_STRING: printf("%s", (char*)CAST(currentByte, StringPointer*)->base.p); break;
			}
			printf("]");
			int typeSize = TypeTable_GetTypeInfo(type)->size;
			printf("(%ub) / ", typeSize);
			currentByte += typeSize;
			type++;
		}
		printf("-> (%ub)", vm.stackTop - vm.stack);
		printf("\n");
		debug_disassemble_opcode(vm.chunk, vm.ip - vm.chunk->code);
#endif

		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{

		case OP_CONSTANT:
		{
			TYPE_ID type = READ_BYTE();
			Byte1* bytes = ByteArray_ReadByte(&vm.chunk->constants, READ_BYTE());
			VM_Push(bytes, type);
			break;
		}

		case OP_DEFINE_GLOBAL:
		{
			StringPointer* name = ByteArray_Read(&vm.chunk->constants, StringPointer*, READ_BYTE());
			TYPE_ID type = READ_BYTE();
			uint8_t varId = READ_BYTE();
			HashTable_Set(&vm.globals, name, &varId, TYPEID_BOOL);
			break;
		}

		case OP_NEGATE:
		{
			double d = -CAST(VM_Pop(TYPEID_DEC), double);
			VM_Push(&d, TYPEID_DEC);
			break;
		}

		case OP_NOT:
		{
			bool b = !CAST(VM_Pop(TYPEID_BOOL), bool);
			VM_Push(&b, TYPEID_BOOL);
			break;
		}

		case OP_ADD:		BINARY_OP(+, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_SUBTRACT:	BINARY_OP(-, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_MULT:		BINARY_OP(*, double, TYPEID_DEC, double, TYPEID_DEC); break;
		case OP_DIVIDE:		BINARY_OP(/ , double, TYPEID_DEC, double, TYPEID_DEC); break;

		case OP_EQUALS: { bool result = Equality(); VM_Push(&result, TYPEID_BOOL); break; }
		case OP_GREAT:		BINARY_OP(> , double, TYPEID_DEC, bool, TYPEID_BOOL); break;
		case OP_LESS:		BINARY_OP(< , double, TYPEID_DEC, bool, TYPEID_BOOL); break;

		case OP_TRUE: { bool b = true; VM_Push(&b, TYPEID_BOOL); break; }
		case OP_FALSE: { bool b = false; VM_Push(&b, TYPEID_BOOL); break; }

					 //case OP_IS:			BINARY_OP(< , bool, TYPEID_BOOL, bool, TYPEID_BOOL); break;

		case OP_PRINT:
		{
			TYPE_ID type = peekType();
			printValue(VM_Pop(type), type);
			printf("\n");
			break;
		}

		case OP_POP: VM_Pop(peekType()); break;
		case OP_RETURN: return INTERPRET_OK;

		}
	}

#undef READ_BYTE
#undef BINARY_OP
}