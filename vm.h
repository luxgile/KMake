#pragma once

#include "bytecode.h"
#include "byteArray.h"
#include "typeTable.h"

#define STACK_MAX 1024

#define CAST(bytes, type) (*(type*)bytes)

typedef struct
{
	ByteCode* chunk;
	uint8_t* ip;
	BYTE* stackTop;
	BYTE stack[STACK_MAX];
	TypeArray stackTypes;
} VM;

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
void VM_Push(BYTE* bytes, TYPE_ID type);
BYTE* VM_Pop(TYPE_ID type);

InterpretResult interpret(const char* source);
InterpretResult run();
