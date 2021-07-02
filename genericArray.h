#pragma once

typedef struct {
	void* elements;
	int elementSize;
	int count;
	int capacity;
} GenericArray;

void genarr_init(GenericArray* arr, int elementSize);
void genarr_free(GenericArray* arr);
void genarr_add(GenericArray* arr, void* element);
inline void* genarr_get(GenericArray* arr, int index);
void* genarr_foreach(GenericArray* arr, void(*f)(void*))