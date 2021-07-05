
#include "genericArray.h"
#include "kmem.h"
#include "common.h"
#include <string.h>

void genarr_init(GenericArray* arr, int elementSize) {
	arr->count = 0;
	arr->capacity = GROW_CAPACITY(0);
	arr->elements = kmem_reallocate(NULL, 0, elementSize * arr->capacity);
	arr->elementSize = elementSize;
}
 
void genarr_free(GenericArray* arr) {
	kmem_reallocate(arr->elements, arr->elementSize * arr->capacity, 0);
	genarr_init(arr, arr->elementSize);
}

void genarr_add(GenericArray* arr, void* element) {
	if (arr->count == arr->capacity) {
		int oldCapacity = arr->capacity;
		arr->capacity = GROW_CAPACITY(arr->capacity);
		arr->elements = kmem_reallocate(arr->elements, arr->elementSize * oldCapacity, arr->elementSize * arr->capacity);
	}

	Byte1* elements = ((Byte1*)arr->elements);
	memcpy(&elements[arr->count * arr->elementSize], element, arr->elementSize);
	arr->count++;
}

void* genarr_get(GenericArray* arr, int index) {
	return (void*)&((Byte1*)arr->elements)[arr->count * arr->elementSize];
}