//#include "hashTable.h"
//
//#include "kmem.h"
//
//#define TABLE_MAX_LOAD 0.75
//
//static KeyValuePair* findEntry(KeyValuePair* entries, int capacity, StringPointer* key)
//{
//	uint32_t index = key->hash % capacity;
//	KeyValuePair* tombstone = NULL;
//	while (true)
//	{
//		KeyValuePair* entry = &entries[index];
//		if (entry->key == NULL) 
//		{
//			if (entry->type == TYPEID_VOID) 
//			{
//				// Empty entry.
//				return tombstone != NULL ? tombstone : entry;
//			}
//			else 
//			{
//				// We found a tombstone.
//				if (tombstone == NULL) tombstone = entry;
//			}
//		}
//		else if (entry->key == key) {
//			// We found the key.
//			return entry;
//		}
//
//		index = (index + 1) % capacity;
//	}
//}
//
//static void adjustCapacity(HashTable* table, int capacity)
//{
//	KeyValuePair* entries = ALLOCATE(KeyValuePair, capacity);
//	for (int i = 0; i < capacity; i++) {
//		entries[i].key = NULL;
//		entries[i].bytes = NULL;
//		entries[i].type = TYPEID_VOID;
//	}
//
//	table->count = 0;
//	for (int i = 0; i < table->capacity; i++)
//	{
//		KeyValuePair* entry = &table->entries[i];
//		if (entry->key == NULL) continue;
//
//		KeyValuePair* dest = findEntry(entries, capacity, entry->key);
//		dest->key = entry->key;
//		dest->bytes = entry->bytes;
//		dest->type = entry->type;
//		table->count++;
//	}
//
//	FREE_ARRAY(KeyValuePair, table->entries, table->capacity);
//
//	table->entries = entries;
//	table->capacity = capacity;
//}
//
//void HashTable_Init(HashTable* ht)
//{
//	ht->count = 0;
//	ht->capacity = 0;
//	ht->entries = NULL;
//}
//
//void HashTable_Free(HashTable* ht)
//{
//	FREE_ARRAY(KeyValuePair, ht->entries, ht->capacity);
//	HashTable_Init(ht);
//}
//
//void HashTable_Set(HashTable* ht, StringPointer* key, Byte1* bytes, TYPE_ID type)
//{
//	if (ht->count + 1 > ht->capacity * TABLE_MAX_LOAD)
//	{
//		int capacity = GROW_CAPACITY(ht->capacity);
//		adjustCapacity(ht, capacity);
//	}
//
//	KeyValuePair* pair = findEntry(ht->entries, ht->capacity, key);
//	bool isNewKey = pair->key == NULL;
//	if (isNewKey && pair->type == TYPEID_VOID) ht->count++;
//
//	pair->key = key;
//	pair->bytes = bytes;
//	pair->type = type;
//	return isNewKey;
//}
//
//bool HashTable_Get(HashTable* table, StringPointer* key, Byte1* bytes, TYPE_ID* type)
//{
//	if (table->count == 0) return false;
//
//	KeyValuePair* entry = findEntry(table->entries, table->capacity, key);
//	if (entry->key == NULL) return false;
//
//	*bytes = *entry->bytes;
//	*type = entry->type;
//	return true;
//}
//
//bool HashTable_Remove(HashTable* table, StringPointer* key)
//{
//	if (table->count == 0) return false;
//
//	// Find the entry.
//	KeyValuePair* entry = findEntry(table->entries, table->capacity, key);
//	if (entry->key == NULL) return false;
//
//	// Place a tombstone in the entry.
//	entry->key = NULL;
//	entry->bytes = NULL;
//	entry->type = TYPEID_BOOL;
//	return true;
//}
//
//void HashTable_AddAll(HashTable* from, HashTable* to)
//{
//	for (int i = 0; i < from->capacity; i++)
//	{
//		KeyValuePair* entry = &from->entries[i];
//		if (entry->key != NULL)
//		{
//			HashTable_Set(to, entry->key, entry->bytes, entry->type);
//		}
//	}
//}
//
//StringPointer* HashTable_Find(HashTable* table, const char* c, int length, uint32_t hash)
//{
//	if (table->count == 0) return NULL;
//
//	uint32_t index = hash % table->capacity;
//	while(true)
//	{
//		KeyValuePair* entry = &table->entries[index];
//		if (entry->key == NULL) 
//		{
//			// Stop if we find an empty non-tombstone entry.
//			if (entry->type == TYPEID_VOID) return NULL;
//		}
//		else if (entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->base.p, c, length) == 0) 
//		{
//			// We found it.
//			return entry->key;
//		}
//
//		index = (index + 1) % table->capacity;
//	}
//}
