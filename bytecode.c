#include <stdlib.h>
#include "bytecode.h"
#include "kmem.h"

void ByteCode_InitChunk(ByteCode *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    ByteArray_Init(&chunk->constants);
    TypeArray_Init(&chunk->types);
    initLinesArray(&chunk->lines);
}

void ByteCode_WriteChunk(ByteCode *chunk, uint8_t byte, int line)
{
    if (chunk->capacity < chunk->count + 1)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    writeLinesArray(&chunk->lines, line);

    chunk->code[chunk->count] = byte;
    chunk->count++;
}

int ByteCode_AddConstant(ByteCode* chunk, TYPE_ID id, BYTE* value)
{
    TypeInfo* type = TypeTable_GetTypeInfo(id);
    ByteArray_AddBytes(&chunk->constants, value, type->size);
    TypeArray_AddType(&chunk->types, id);
    return chunk->constants.count - type->size;
}

void ByteCode_FreeChunk(ByteCode* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    ByteArray_Free(&chunk->constants);
    TypeArray_Free(&chunk->types);
    freeLinesArray(&chunk->lines);
    ByteCode_InitChunk(chunk);
}