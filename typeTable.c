#include "typeTable.h"
#include "commonTypes.h"
#include "common.h"
#include <string.h>
#include "kmem.h"

void typetbl_init(TypeTable* table)
{
	table->count = 0;
	table->capacity = 0;
	table->types = NULL;

	//Dec = 0
	TypeInfo voidInfo;
	voidInfo.name = "Void";
	voidInfo.size = 0;
	typetbl_add(table, voidInfo);

	//Dec = 1
	TypeInfo doubleInfo;
	doubleInfo.name = "Dec";
	doubleInfo.size = sizeof(double);
	typetbl_add(table, doubleInfo);

	//Bool = 2
	TypeInfo boolInfo;
	boolInfo.name = "Bool";
	boolInfo.size = sizeof(bool);
	typetbl_add(table, boolInfo);

	//Bool = 3
	TypeInfo pInfo;
	pInfo.name = "Pointer";
	pInfo.size = sizeof(Pointer);
	typetbl_add(table, pInfo);

	//Bool = 4
	TypeInfo stringInfo;
	stringInfo.name = "String";
	stringInfo.size = sizeof(StringPointer);
	typetbl_add(table, stringInfo);
}

TYPE_ID typetbl_add(TypeTable* table, TypeInfo type)
{
	if (table->capacity < table->count + 1)
	{
		int oldCapacity = table->capacity;
		table->capacity = GROW_CAPACITY(oldCapacity);
		table->types = GROW_ARRAY(TypeInfo, table->types, oldCapacity, table->capacity);
	}

	table->types[table->count] = type;
	table->count++;
	return table->count - 1;
}

TypeInfo* typetbl_get_info(TypeTable* table, TYPE_ID id)
{
	return &table->types[id];
}

TYPE_ID typetbl_get_id(TypeTable* table, const char* name, int length)
{
	for (TYPE_ID i = 0; i < table->count; i++)
	{
		if (memcmp(table->types[i].name, name, length) == 0)
			return i;
	}

	return TYPEID_VOID;
}

void typetbl_free(TypeTable* table)
{
	FREE_ARRAY(TypeInfo, table->types, table->capacity);
	typetbl_init(table);
}

void typearr_init(TypeArray* typeArray)
{
	typeArray->count = 0;
	typeArray->capacity = 0;
	typeArray->types = NULL;
}

void typearr_add(TypeArray* typeArray, TYPE_ID type)
{
	if (typeArray->capacity < typeArray->count + 1)
	{
		int oldCapacity = typeArray->capacity;
		typeArray->capacity = GROW_CAPACITY(oldCapacity);
		typeArray->types = GROW_ARRAY(TYPE_ID, typeArray->types, oldCapacity, typeArray->capacity);
	}

	typeArray->types[typeArray->count] = type;
	typeArray->count++;
}

void typearr_remove(TypeArray* typeArray, TYPE_ID type)
{
	int index = -1;
	for (int i = 0; i < typeArray->count; i++)
	{
		if (typeArray->types[i] == type)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
		return;

	for (int i = index; i < typeArray->count - 1; i++)
		typeArray->types[i] = typeArray->types[i + 1];

	typeArray->count--;
}

void typearr_free(TypeArray* typeArray)
{
	FREE_ARRAY(TYPE_ID, typeArray->types, typeArray->capacity);
	typearr_init(typeArray);
}
