#include "commonTypes.h"
#include <stdio.h>
#include "kmem.h"
#include "vm.h"

bool ktype_equality(Byte1* a, Byte1* b, TYPE_ID type) {
	switch (type) {
	case TYPEID_BOOL: return *(bool*)a == *(bool*)b;
	case TYPEID_DEC: return *(double*)a == *(double*)b;
		//case TYPEID_STRING: return *(double*)a == *(double*)b;
	}

	return false;
}

void ktype_print(Byte1* value, TYPE_ID type) {
	switch (type) {
	case TYPEID_DEC: printf("%g", ktype_double(value)); break;
	case TYPEID_BOOL: printf("%s", ktype_bool(value) ? "true" : "false"); break;
	default: printf("var type: %u", type);
		//case TYPEID_STRING: printf("%s", (char*)CAST(bytes, StringPointer*)->base.p); break;
	}
}

double ktype_double(Byte1* value) {
	return *(double*)value;
}

bool ktype_bool(Byte1* value) {
	return *(bool*)value;
}
