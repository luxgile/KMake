#ifndef __KMEM_HEADER___
#define __KMEM_HEADER___

#include "common.h"

#define GROW_CAPACITY(capacity) \
((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type *)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0);

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

void* reallocate(void* pointer, size_t oldCount, size_t newCount);

#endif