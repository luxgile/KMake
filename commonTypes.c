#include "commonTypes.h"
#include "kmem.h"
#include <string.h>

static Pointer* AllocateObject(size_t size, TYPE_ID type)
{
	Pointer* object = (Pointer*)reallocate(NULL, 0, size);
	object->type = type;
	return object;
}

static StringPointer* AllocateString(char* chars, int length, uint32_t hash)
{
	StringPointer* string = (StringPointer*)AllocateObject(sizeof(StringPointer), TYPEID_STRING);
	string->length = length;
	string->base.p = chars;
	string->hash = hash;
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
	return AllocateString(heapChars, length);
}
