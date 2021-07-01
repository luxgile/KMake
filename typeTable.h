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

void typetbl_init();
TYPE_ID typetbl_add(TypeInfo byte);
TypeInfo* typetbl_get_info(TYPE_ID id);
TYPE_ID typetbl_get_id(char* name, int length);
void typetbl_free();

void typearr_init(TypeArray* typeArray);
void typearr_add(TypeArray* typeArray, TYPE_ID type);
void typearr_remove(TypeArray* typeArray, TYPE_ID type);
void typearr_free(TypeArray* typeArray);