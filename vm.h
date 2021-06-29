#pragma once

#include "bytecode.h"
#include "byteArray.h"
#include "typeTable.h"
#include "hashTable.h"

#define STACK_MAX 1024

#define CAST(bytes, type) (*(type*)bytes)

typedef struct
{
	ByteCode* chunk;
	uint8_t* ip;
	Byte1* stackTop;
	Byte1 stack[STACK_MAX];
	TypeArray stackTypes;
	HashTable strings;
	HashTable globals;
} VM;

VM vm;

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
void VM_Push(Byte1* bytes, TYPE_ID type);
Byte1* VM_Pop(TYPE_ID type);

InterpretResult interpret(const char* source);
InterpretResult run();
