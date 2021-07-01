#include "typeTable.h"
#include "commonTypes.h"
#include <string.h>
#include "kmem.h"

void typetbl_init()
{
	Global_TT.count = 0;
	Global_TT.capacity = 0;
	Global_TT.types = NULL;

	//Dec = 0
	TypeInfo voidInfo;
	voidInfo.name = "Void";
	voidInfo.size = 0;
	typetbl_add(voidInfo);

	//Dec = 1
	TypeInfo doubleInfo;
	doubleInfo.name = "Dec";
	doubleInfo.size = sizeof(double);
	typetbl_add(doubleInfo);

	//Bool = 2
	TypeInfo boolInfo;
	boolInfo.name = "Bool";
	boolInfo.size = sizeof(bool);
	typetbl_add(boolInfo);

	//Bool = 3
	TypeInfo pInfo;
	pInfo.name = "Pointer";
	pInfo.size = sizeof(Pointer);
	typetbl_add(pInfo);

	//Bool = 4
	TypeInfo stringInfo;
	stringInfo.name = "String";
	stringInfo.size = sizeof(StringPointer);
	typetbl_add(stringInfo);
}

TYPE_ID typetbl_add(TypeInfo type)
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

TypeInfo* typetbl_get_info(TYPE_ID id)
{
	return &Global_TT.types[id];
}

TYPE_ID typetbl_get_id(char* name, int length)
{
	for (size_t i = 0; i < Global_TT.count; i++)
	{
		if (memcmp(Global_TT.types[i].name, name, length) == 0)
			return i;
	}

	return TYPEID_VOID;
}

void typetbl_free()
{
	FREE_ARRAY(TypeInfo, Global_TT.types, Global_TT.capacity);
	typetbl_init();
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

void typearr_free(TypeArray* typeArray)
{
	FREE_ARRAY(TYPE_ID, typeArray->types, typeArray->capacity);
	typearr_init(typeArray);
}
