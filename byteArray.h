#pragma once

#include "common.h"


typedef struct
{
	int count;
	int capacity;
	Byte1* bytes;
} ByteArray;

void ByteArray_Init(ByteArray* array);
void ByteArray_AddByte(ByteArray* array, Byte1 byte);
void ByteArray_AddBytes(ByteArray* array, Byte1* bytes, int size);
void ByteArray_Free(ByteArray* array);

#define ByteArray_Read(array, type, index) (*(type*)&(array)->bytes[index])
#define ByteArray_ReadByte(array, index) &(array)->bytes[index]
