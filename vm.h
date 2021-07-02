#pragma once

#include "bytecode.h"
#include "byteArray.h"
#include "byteStack.h"
#include "typeTable.h"
#include "hashTable.h"
#include "opcodes.h"

#define STACK_MAX 1024

#define CAST(bytes, type) (*(type*)bytes)

typedef struct
{
	ByteCode* chunk;
	uint8_t* ip;

	ByteStack stack;
	TypeArray stackTypes;

	OpCodeRegister opReg;
	TypeTable typeTable;
	HashTable strings;
	HashTable globals;
} VM;

typedef enum
{
	INTERPRET_NONE,
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void vm_init(VM* vm);
void vm_free(VM* vm);

void vm_push(VM* vm, Byte1* value, TYPE_ID type, int size);
void vm_pop(VM* vm, Byte1* out_value, TYPE_ID type, int size);

inline TYPE_ID vm_peek_type(VM* vm);
inline TYPE_ID vm_peek_typec(VM* vm, int depth);
inline Byte1* vm_read_ip(VM* vm);

//void vm_push(VM* vm, Byte1* bytes, TYPE_ID type);
//Byte1* vm_pop(VM* vm, TYPE_ID type);

InterpretResult vm_interpret(VM* vm, const char* source);
//InterpretResult vm_run();
