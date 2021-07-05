#ifndef __GENERIC_ARRAY_HEADER__
#define __GENERIC_ARRAY_HEADER__

typedef struct GenericArray {
	void* elements;
	int elementSize;
	int count;
	int capacity;
} GenericArray;

void genarr_init(GenericArray* arr, int elementSize);
void genarr_free(GenericArray* arr); 
void genarr_add(GenericArray* arr, void* element);
void* genarr_get(GenericArray* arr, int index);

#endif