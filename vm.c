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

VM vm;


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

void initVM()
{
	resetStack();

	TypeArray_Init(&vm.stackTypes);
}

void freeVM()
{
	TypeArray_Free(&vm.stackTypes);
}

void VM_Push(BYTE* bytes, TYPE_ID type)
{
	TypeArray_AddType(&vm.stackTypes, type);

	int size = TypeTable_GetTypeInfo(type)->size;
	memcpy(vm.stackTop, bytes, size);
	vm.stackTop += size;
}

BYTE* VM_Pop(TYPE_ID type)
{
	TypeArray_RemoveType(&vm.stackTypes, type);

	vm.stackTop -= TypeTable_GetTypeInfo(type)->size;
	return vm.stackTop;
}
BYTE* VM_PopPointer(TYPE_ID type)
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
	ByteCode_InitChunk(&c);

	if (!compile(source, &c))
	{
		ByteCode_FreeChunk(&c);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &c;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	ByteCode_FreeChunk(&c);
	return result;
	//return run();
}

static bool Equality()
{
	switch (peekType())
	{
	case TYPEID_BOOL: return CAST(VM_Pop(TYPEID_BOOL), bool) == CAST(VM_Pop(TYPEID_BOOL), bool);
	case TYPEID_DEC: return CAST(VM_Pop(TYPEID_DEC), double) == CAST(VM_Pop(TYPEID_DEC), double);
	case TYPEID_STRING:
	{
		StringPointer* a = CAST(VM_Pop(TYPEID_STRING), StringPointer*);
		StringPointer* b = CAST(VM_Pop(TYPEID_STRING), StringPointer*);
		return a->length == b->length && memcmp(a->base.p, b->base.p, a->length) == 0;
	}
	}

	return false;
}

InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op, intype, inid, outtype, outid) \
    do { \
      intype b = CAST(VM_Pop(inid), intype); \
      intype a = CAST(VM_Pop(inid), intype); \
	  outtype result = a op b; \
      VM_Push(&result, outid); \
    } while (false)

	for (;;)
	{

#ifdef DEBUG_TRACE_EXECUTION

		printf("\t\t");
		BYTE* currentByte = &vm.stack;
		for (int i = 0; i < vm.stackTypes.count; i++)
		{
			TYPE_ID type = vm.stackTypes.types[i];
			printf("[");
			switch (type)
			{
			case TYPEID_DEC: printf("%g", CAST(currentByte, double)); break;
			case TYPEID_BOOL: printf("%s", CAST(currentByte, bool) ? "true" : "false"); break;
			case TYPEID_STRING: printf("%s", (char*)CAST(currentByte, StringPointer*)->base.p); break;
			}
			printf("]");
			currentByte += TypeTable_GetTypeInfo(type)->size;
			type++;
		}
		printf("\n");
		disassembleInstruction(vm.chunk, vm.ip - vm.chunk->code);
#endif

		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{

		case OP_CONSTANT:
		{
			TYPE_ID type = READ_BYTE();
			BYTE* bytes = ByteArray_ReadByte(&vm.chunk->constants, READ_BYTE());
			VM_Push(bytes, type);
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

		case OP_RETURN:
		{
			switch (peekType())
			{
			case TYPEID_DEC: printf("%g", CAST(VM_Pop(TYPEID_DEC), double)); break;
			case TYPEID_BOOL: printf("%s", CAST(VM_Pop(TYPEID_BOOL), bool) ? "true" : "false"); break;
			case TYPEID_STRING: printf("%s", (char*)CAST(VM_Pop(TYPEID_STRING), StringPointer*)->base.p); break;
			}
			printf("\n");
			return INTERPRET_OK;
		}

		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}