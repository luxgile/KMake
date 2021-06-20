#pragma once

#include "common.h"

typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_DEC,
} FieldType;

typedef struct
{
	FieldType type;
	union
	{
		bool b;
		double d;
	} as;
} Field;

typedef struct
{
	int capacity;
	int count;
	Field* values;
} FieldArray;

void initFieldArray(FieldArray* array);
void writeFieldArray(FieldArray* array, Field value);
void freeFieldArray(FieldArray* array);
void printField(Field dec);

#define BOOL_VAL(value)   ((Field){VAL_BOOL, {.b = value}})
#define DEC_VAL(value) ((Field){VAL_DEC, {.d = value}})

#define AS_BOOL(value)    ((value).as.b)
#define AS_DEC(value)  ((value).as.d)

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_DEC(value)  ((value).type == VAL_DEC)
