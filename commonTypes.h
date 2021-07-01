#pragma once

#include "typeTable.h"

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
	TYPE_ID type;
	void* p;
} Pointer;

typedef struct
{
	Pointer base;
	int length;
	uint32_t hash;
} StringPointer;

StringPointer* CopyString(const char* chars, int length);
bool ktype_equality(Byte1* a, Byte1* b, TYPE_ID type);
void ktype_print(Byte1* value, TYPE_ID type);

inline double ktype_double(Byte1* value) { return *(double*)value; }
inline bool ktype_bool(Byte1* value) { return *(bool*)value; }