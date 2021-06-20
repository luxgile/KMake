#ifndef __BYTECODE_HEADER__
#define __BYTECODE_HEADER__

#include "common.h"
#include "byteArray.h"
#include "runLines.h"
#include "typeTable.h"

typedef enum
{
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULT,
	OP_DIVIDE,
	OP_TRUE,
	OP_FALSE,
	OP_NOT,
	OP_RETURN,
} OpCode;

typedef struct
{
	int count;
	int capacity;
	uint8_t* code;
	ByteArray constants;
	TypeArray types;
	RunLineArray lines;
} ByteCode;

void ByteCode_InitChunk(ByteCode* chunk);
void ByteCode_WriteChunk(ByteCode* chunk, uint8_t code, int line);
int ByteCode_AddConstant(ByteCode* chunk, TYPE_ID id, BYTE* value);
void ByteCode_FreeChunk(ByteCode* chunk);

#endif
