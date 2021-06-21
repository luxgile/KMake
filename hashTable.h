#pragma once

#include "commonTypes.h"

typedef struct
{
	StringPointer* key;
	TYPE_ID type;
	BYTE* bytes;
} KeyValuePair;

typedef struct
{
	int count;
	int capacity;
	KeyValuePair* entries;
} HashTable;

void HashTable_Init(HashTable* ht);
void HashTable_Free(HashTable* ht);