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
} StringPointer;

StringPointer* CopyString(const char* chars, int length);