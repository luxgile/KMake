#pragma once

#include "common.h"

typedef uint8_t BYTE;

typedef struct
{
	int count;
	int capacity;
	BYTE* bytes;
} ByteArray;

void ByteArray_Init(ByteArray* array);
void ByteArray_AddByte(ByteArray* array, BYTE byte);
void ByteArray_AddBytes(ByteArray* array, BYTE* bytes, int size);
void ByteArray_Free(ByteArray* array);

#define ByteArray_Read(array, type, index) *(type*)&(array)->bytes[index]
#define ByteArray_ReadByte(array, index) &(array)->bytes[index]
