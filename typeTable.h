#pragma once

#include "byteArray.h"

typedef uint16_t TYPE_ID;
typedef void (*DebugValueFn)(BYTE* bytes);

#define TYPEID_DEC 0
#define IS_DEC(type) type == TYPEID_DEC

#define TYPEID_BOOL 1
#define IS_BOOL(type) type == TYPEID_BOOL

#define TYPEID_POINTER 2
#define IS_POINTER(type) type == TYPEID_POINTER

#define TYPEID_STRING 3
#define IS_POINTER(type) type == TYPEID_POINTER

typedef struct
{
	char* name;
	int size;
	DebugValueFn Debug;
} TypeInfo;

typedef struct
{
	int count;
	int capacity;
	TypeInfo* types;
} TypeTable;

typedef struct
{
	int count;
	int capacity;
	TYPE_ID* types;
} TypeArray;

TypeTable Global_TT;

void TypeTable_Init();
TYPE_ID TypeTable_AddType(TypeInfo byte);
TypeInfo* TypeTable_GetTypeInfo(TYPE_ID id);
int TypeTable_GetTypeId(char* name);
void TypeTable_Free();


void TypeArray_Init(TypeArray* typeArray);
void TypeArray_AddType(TypeArray* typeArray, TYPE_ID type);
void TypeArray_RemoveType(TypeArray* typeArray, TYPE_ID type);
void TypeArray_Free(TypeArray* typeArray);