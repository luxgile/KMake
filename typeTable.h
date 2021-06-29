#pragma once

#include "byteArray.h"

typedef uint8_t TYPE_ID;
typedef void (*DebugValueFn)(Byte1* bytes);

#define TYPEID_VOID 0
#define IS_VOID(type) type == TYPEID_VOID

#define TYPEID_DEC 1
#define IS_DEC(type) type == TYPEID_DEC

#define TYPEID_BOOL 2
#define IS_BOOL(type) type == TYPEID_BOOL

#define TYPEID_POINTER 3
#define IS_POINTER(type) type == TYPEID_POINTER

#define TYPEID_STRING 4
#define IS_STRING(type) type == TYPEID_STRING

typedef struct
{
	char* name;
	int size;
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
TYPE_ID TypeTable_GetTypeId(char* name, int length);
void TypeTable_Free();


void TypeArray_Init(TypeArray* typeArray);
void TypeArray_AddType(TypeArray* typeArray, TYPE_ID type);
void TypeArray_RemoveType(TypeArray* typeArray, TYPE_ID type);
void TypeArray_Free(TypeArray* typeArray);