#include "commonTypes.h"
#include <stdio.h>
#include <string.h>
#include "kmem.h"
#include "vm.h"

String ktype_string_create(const char* chars, int length) {
	char* heap = ALLOCATE(char, length + 1);
	memcpy(heap, chars, length);
	heap[length] = '\0';
	String s;
	s.chars = heap;
	s.length = length;
	return s;
}

bool ktype_equality(Byte1* a, Byte1* b, TYPE_ID type) {
	switch (type) {
	case TYPEID_BOOL: return *(bool*)a == *(bool*)b;
	case TYPEID_DEC: return *(double*)a == *(double*)b;
	case TYPEID_STRING:
	{
		String as = *(String*)a;
		String bs = *(String*)b;
		if (&as == &bs) return true;
		else if (as.length != bs.length) return false;
		else return memcmp(as.chars, bs.chars, as.length) == 0;
	}
	}

	return false;
}

void ktype_print(Byte1* value, TYPE_ID type) {
	switch (type) {
	case TYPEID_DEC: printf("%g", ktype_double(value)); break;
	case TYPEID_BOOL: printf("%s", ktype_bool(value) ? "true" : "false"); break;
	case TYPEID_STRING: printf("%s", ((String*)value)->chars); break;
	default: printf("var type: %u", type);
	}
}

double ktype_double(Byte1* value) {
	return *(double*)value;
}

bool ktype_bool(Byte1* value) {
	return *(bool*)value;
}
