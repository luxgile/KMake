#include "hashTable.h"

#include "kmem.h"

void HashTable_Init(HashTable* ht)
{
	ht->count = 0;
	ht->capacity = 0;
	ht->entries = NULL;
}

void HashTable_Free(HashTable* ht)
{
	FREE_ARRAY(KeyValuePair, ht->entries, ht->capacity);
	HashTable_Init(ht);
}
