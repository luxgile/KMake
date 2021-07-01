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

typedef enum {
	ERR_NONE,
	ERR_STACK_OVERFLOW,
	ERR_STACK_UNDERFLOW,
} VMError;

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void vm_init(VM* vm);
void vm_free(VM* vm);
//void vm_push(VM* vm, Byte1* bytes, TYPE_ID type);
//Byte1* vm_pop(VM* vm, TYPE_ID type);

InterpretResult vm_interpret(VM* vm, const char* source);
//InterpretResult vm_run();
