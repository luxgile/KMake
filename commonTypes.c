#include "commonTypes.h"
#include "kmem.h"
#include "vm.h"
#include <string.h>

static Pointer* AllocateObject(size_t size, TYPE_ID type)
{
	Pointer* object = (Pointer*)kmem_reallocate(NULL, 0, size);
	object->type = type;
	return object;
}

static StringPointer* AllocateString(char* chars, int length, uint32_t hash)
{
	StringPointer* string = (StringPointer*)AllocateObject(sizeof(StringPointer), TYPEID_STRING);
	string->length = length;
	string->base.p = chars;
	string->hash = hash;
	HashTable_Set(&vm.strings, string, chars, TYPEID_STRING);
	return string;
}

static uint32_t HashString(const char* key, int length)
{
	uint32_t hash = 2166136261u;
	for (int i = 0; i < length; i++)
	{
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

StringPointer* CopyString(const char* chars, int length)
{
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	uint32_t hash = HashString(chars, length);
	StringPointer* interned = HashTable_Find(&vm.strings, heapChars, length, hash);
	if (interned != NULL) return interned;
	return AllocateString(heapChars, length, hash);
}

bool ktype_equality(Byte1* a, Byte1* b, TYPE_ID type)
{
	switch (type)
	{
	case TYPEID_BOOL: return *(bool*)a == *(bool*)b;
	case TYPEID_DEC: return *(double*)a == *(double*)b;
	//case TYPEID_STRING: return *(double*)a == *(double*)b;
	}

	return false;
}

void ktype_print(Byte1* value, TYPE_ID type)
{
	switch (type)
	{
	case TYPEID_DEC: printf("%g", ktype_double(value)); break;
	case TYPEID_BOOL: printf("%s", ktype_bool(value) ? "true" : "false"); break;
	//case TYPEID_STRING: printf("%s", (char*)CAST(bytes, StringPointer*)->base.p); break;
	}
}