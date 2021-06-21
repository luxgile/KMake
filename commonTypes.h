#pragma once

#include "typeTable.h"

typedef struct
{
	TYPE_ID type;
	void* p;
} Pointer;

typedef struct
{
	Pointer base;
	int length;
	uint32_t hash;
} StringPointer;

StringPointer* CopyString(const char* chars, int length);