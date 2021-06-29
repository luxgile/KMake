#include "byteArray.h"

#include <string.h>

#include "kmem.h"

void ByteArray_Init(ByteArray* array)
{
	array->count = 0;
	array->capacity = 0;
	array->bytes = NULL;
}

void ByteArray_AddByte(ByteArray* array, Byte1 byte)
{
	if (array->capacity < array->count + 1)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->bytes = GROW_ARRAY(Byte1, array->bytes, oldCapacity, array->capacity);
	}

	array->bytes[array->count] = byte;
	array->count++;
}

void ByteArray_AddBytes(ByteArray* array, Byte1* bytes, int size)
{
	if (array->capacity < array->count + size)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->bytes = GROW_ARRAY(Byte1, array->bytes, oldCapacity, array->capacity);
		ByteArray_AddBytes(array, bytes, size);
		return;
	}

	memcpy(&array->bytes[array->count], bytes, size);

	array->count += size;
}

void ByteArray_Free(ByteArray* array)
{
	FREE_ARRAY(uint8_t, array->bytes, array->capacity);
	ByteArray_Init(array);
}
