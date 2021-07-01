#pragma once

#include "common.h"


typedef struct
{
	int count;
	int capacity;
	Byte1* bytes;
} ByteArray;

void bytearr_init(ByteArray* array);
void bytearr_add(ByteArray* array, Byte1 byte);
void bytearr_addarr(ByteArray* array, Byte1* bytes, int size);
void bytearr_free(ByteArray* array);

#define ByteArray_Read(array, type, index) (*(type*)&(array)->bytes[index])
#define ByteArray_ReadByte(array, index) &(array)->bytes[index]
