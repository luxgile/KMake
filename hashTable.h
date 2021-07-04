//#pragma once
//
//#include "commonTypes.h"
//
//typedef struct
//{
//	StringPointer* key;
//	TYPE_ID type;
//	Byte1* bytes;
//} KeyValuePair;
//
//typedef struct
//{
//	int count;
//	int capacity;
//	KeyValuePair* entries;
//} HashTable;
//
//void HashTable_Init(HashTable* ht);
//void HashTable_Free(HashTable* ht);
//
//void HashTable_Set(HashTable* ht, StringPointer* key, Byte1* bytes, TYPE_ID type);
//bool HashTable_Get(HashTable* table, StringPointer* key, Byte1* bytes, TYPE_ID* type);
//bool HashTable_Remove(HashTable* table, StringPointer* key);
//void HashTable_AddAll(HashTable* from, HashTable* to);
//StringPointer* HashTable_Find(HashTable* table, const char* c, int length, uint32_t hash);