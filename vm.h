#ifndef __VIRTUAL_MACHIN_HEADER__
#define __VIRTUAL_MACHIN_HEADER__

#include "bytecode.h"
#include "byteStack.h"
#include "typeTable.h"
#include "hashTable.h"
#include "opcodes.h"

struct OpCodeRegister;

typedef struct VM {
	ByteCode* chunk;
	Byte1* ip;

	ByteStack stack;
	TypeArray stackTypes;

	OpCodeRegister opReg;
	TypeTable typeTable;
	//HashTable strings;
	//HashTable globals;
} VM;

typedef enum {
	INTERPRET_NONE,
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void vm_init(VM* vm);
void vm_free(VM* vm);

void vm_push(VM* vm, Byte1* value, TYPE_ID type, int size);
void vm_pop(VM* vm, Byte1** out_value, TYPE_ID type, int size);

TYPE_ID vm_peek_type(VM* vm);
TYPE_ID vm_peek_typec(VM* vm, int depth);
Byte1 vm_read_ip(VM* vm);

InterpretResult vm_interpret(VM* vm, const char* source);

#endif