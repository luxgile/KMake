#include <stdlib.h>
#include "kmem.h"

void* kmem_reallocate(void* pointer, size_t oldSize, size_t newSize)
{
    if(newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, newSize);
    if(result == NULL) exit(EXIT_FAILURE);
    return result;
}