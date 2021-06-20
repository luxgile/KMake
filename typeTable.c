#include "typeTable.h"

#include <string.h>
#include "kmem.h"

void TypeTable_Init()
{
	Global_TT.count = 0;
	Global_TT.capacity = 0;
	Global_TT.types = NULL;

	//Dec = 0
	TypeInfo doubleInfo;
	doubleInfo.name = "Dec";
	doubleInfo.size = sizeof(double);
	TypeTable_AddType(doubleInfo);

	//Bool = 0
	TypeInfo boolInfo;
	boolInfo.name = "Bool";
	boolInfo.size = sizeof(bool);
	TypeTable_AddType(boolInfo);
}

TYPE_ID TypeTable_AddType(TypeInfo type)
{
	if (Global_TT.capacity < Global_TT.count + 1)
	{
		int oldCapacity = Global_TT.capacity;
		Global_TT.capacity = GROW_CAPACITY(oldCapacity);
		Global_TT.types = GROW_ARRAY(TypeInfo, Global_TT.types, oldCapacity, Global_TT.capacity);
	}

	Global_TT.types[Global_TT.count] = type;
	Global_TT.count++;
	return Global_TT.count - 1;
}

TypeInfo* TypeTable_GetTypeInfo(TYPE_ID id)
{
	return &Global_TT.types[id];
}

int TypeTable_GetTypeId(char* name)
{
	return 0;
}

void TypeTable_Free()
{
	FREE_ARRAY(TypeInfo, Global_TT.types, Global_TT.capacity);
	TypeTable_Init();
}

void TypeArray_Init(TypeArray* typeArray)
{
	typeArray->count = 0;
	typeArray->capacity = 0;
	typeArray->types = NULL;
}

void TypeArray_AddType(TypeArray* typeArray, TYPE_ID type)
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

void TypeArray_RemoveType(TypeArray* typeArray, TYPE_ID type)
{
	int index = -1;
	for (size_t i = 0; i < typeArray->count; i++)
	{
		if (typeArray->types[i] == type)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
		return;

	for (size_t i = index; i < typeArray->count - 1; i++)
		typeArray->types[i] = typeArray->types[i + 1];

	typeArray->count--;
}

void TypeArray_Free(TypeArray* typeArray)
{
	FREE_ARRAY(TYPE_ID, typeArray->types, typeArray->capacity);
	TypeArray_Init(typeArray);
}
