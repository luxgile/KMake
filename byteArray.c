#include "byteArray.h"

#include <string.h>

#include "kmem.h"

void bytearr_init(ByteArray* array)
{
	array->count = 0;
	array->capacity = 0;
	array->bytes = NULL;
}

void bytearr_add(ByteArray* array, Byte1 byte)
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

void bytearr_addarr(ByteArray* array, Byte1* bytes, int size)
{
	if (array->capacity < array->count + size)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->bytes = GROW_ARRAY(Byte1, array->bytes, oldCapacity, array->capacity);
		bytearr_addarr(array, bytes, size);
		return;
	}

	memcpy(&array->bytes[array->count], bytes, size);

	array->count += size;
}

inline Byte1* bytearr_read(ByteArray* array, int index)
{
	return &(array)->bytes[index];
}

void bytearr_free(ByteArray* array)
{
	FREE_ARRAY(uint8_t, array->bytes, array->capacity);
	bytearr_init(array);
}
