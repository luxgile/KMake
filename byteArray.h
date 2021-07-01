#pragma once

#include "common.h"


typedef struct
{
	int count;
	int capacity;
	Byte1* bytes;
} ByteArray;

void bytearr_init(ByteArray* array);
void bytearr_free(ByteArray* array);
void bytearr_add(ByteArray* array, Byte1 byte);
void bytearr_addarr(ByteArray* array, Byte1* bytes, int size);
inline Byte1* bytearr_read(ByteArray* array, int size);

#define bytearr_read_type(array, type, index) ((type*)bytearr_read(array, index));
