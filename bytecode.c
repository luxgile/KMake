#include <stdlib.h>
#include "bytecode.h"
#include "kmem.h"

void bytec_init_chunk(ByteCode* chunk)
{
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;

	bytearr_init(&chunk->constants);
	typearr_init(&chunk->types);
	linearr_init(&chunk->lines);
}

void bytec_write(ByteCode* chunk, uint8_t byte, int line)
{
	if (chunk->capacity < chunk->count + 1)
	{
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
	}

	linearr_write(&chunk->lines, line);

	chunk->code[chunk->count] = byte;
	chunk->count++;
}

int bytec_add_c(ByteCode* chunk, Byte1* value, TYPE_ID type, int size)
{
	bytearr_addarr(&chunk->constants, value, size);
	typearr_add(&chunk->types, type);
	return chunk->constants.count - size;
}

//int bytec_add_cp(ByteCode* chunk, TYPE_ID id, Byte1* pointer)
//{
//	size_t size = sizeof(void*);
//	bytearr_addarr(&chunk->constants, &pointer, size);
//	typearr_add(&chunk->types, id);
//	return chunk->constants.count - size;
//}

void bytec_free(ByteCode* chunk)
{
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	bytearr_free(&chunk->constants);
	typearr_free(&chunk->types);
	linearr_free(&chunk->lines);
	ByteCode_InitChunk(chunk);
}