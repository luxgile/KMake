#pragma once

#include <stdio.h>
#include "values.h"
#include "kmem.h"

void initFieldArray(FieldArray* array)
{
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void writeFieldArray(FieldArray* array, Field value)
{
	if (array->capacity < array->count + 1)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values = GROW_ARRAY(Field, array->values,
			oldCapacity, array->capacity);
	}

	array->values[array->count] = value;
	array->count++;
}

void freeFieldArray(FieldArray* array)
{
	FREE_ARRAY(Field, array->values, array->capacity);
	initFieldArray(array);
}

void printField(Field dec)
{
	switch (dec.type)
	{
	case VAL_BOOL: printf(AS_BOOL(dec) ? "true" : "false"); break;
	case VAL_DEC: printf("%g", AS_DEC(dec)); break;
	}
}
