#pragma once

#include "byteArray.h"

typedef uint8_t TYPE_ID;

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

void typetbl_init(TypeTable* table);
TYPE_ID typetbl_add(TypeTable* table, TypeInfo byte);
TypeInfo* typetbl_get_info(TypeTable* table, TYPE_ID id);
TYPE_ID typetbl_get_id(TypeTable* table, char* name, int length);
void typetbl_free(TypeTable* table);

void typearr_init(TypeArray* typeArray);
void typearr_add(TypeArray* typeArray, TYPE_ID type);
void typearr_remove(TypeArray* typeArray, TYPE_ID type);
void typearr_free(TypeArray* typeArray);