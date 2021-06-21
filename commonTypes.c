#include "commonTypes.h"
#include "kmem.h"
#include <string.h>

static Pointer* AllocateObject(size_t size, TYPE_ID type) 
{
	Pointer* object = (Pointer*)reallocate(NULL, 0, size);
	object->type = type;
	return object;
}

static StringPointer* AllocateString(char* chars, int length)
{
	StringPointer* string = (StringPointer*)AllocateObject(sizeof(StringPointer), TYPEID_STRING);
	string->length = length;
	string->base.p = chars;
	return string;
}

StringPointer* CopyString(const char* chars, int length)
{
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	return AllocateString(heapChars, length);
}
