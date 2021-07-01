#ifndef __BYTECODE_HEADER__
#define __BYTECODE_HEADER__

#include "common.h"
#include "byteArray.h"
#include "runLines.h"
#include "typeTable.h"

typedef struct
{
	int count;
	int capacity;
	uint8_t* code;
	ByteArray constants;
	TypeArray types;
	RunLineArray lines;
} ByteCode;

void bytec_init_chunk(ByteCode* chunk);
void bytec_write(ByteCode* chunk, uint8_t code, int line);
int bytec_add_c(ByteCode* chunk, TYPE_ID id, Byte1* value);
int bytec_add_cp(ByteCode* chunk, TYPE_ID id, Byte1* value);
void bytec_free(ByteCode* chunk);

#endif
