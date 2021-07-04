
#include "byteStack.h"
#include "kmem.h"
#include <stdio.h>
#include <string.h>

void bytestk_init(ByteStack* stack, int capacity) {
	stack->capacity = capacity;
	stack->stack = ALLOCATE(Byte1, capacity);
	stack->stacktop = stack->stack;
}

void bytestk_free(ByteStack* stack) {
	FREE_ARRAY(Byte1, stack->stack, stack->capacity);
	bytestk_init(stack, 0);
}

inline int bytestk_count(ByteStack* stack) {
	return (int)(stack->stacktop - stack->stack);
}

void bytestk_push(ByteStack* stack, Byte1 byte) {
#ifdef DEBUG_SAFETY
	if (bytestk_count(stack) == stack->capacity) {
		printf("Error: Stack Overflow.\n");
		return;
	}
#endif

	* stack->stacktop = byte;
	stack->stacktop++;
}

void bytestk_push_arr(ByteStack* stack, Byte1* arr, int count) {
#ifdef DEBUG_SAFETY
	if (bytestk_count(stack) + count > stack->capacity) {
		printf("Error: Stack Overflow.\n");
		return;
	}
#endif

	memcpy(stack->stacktop, arr, count);
	stack->stacktop += count;
}

Byte1 bytestk_pop(ByteStack* stack) {
#ifdef DEBUG_SAFETY
	if (bytestk_count(stack) == 0) printf("Error: Stack Underflow.");
#endif

	stack->stacktop--;
	return *stack->stacktop;
}

void bytestk_pop_arr(ByteStack* stack, Byte1** out_arr, int count) {
	if (out_arr == NULL) return;
#ifdef DEBUG_SAFETY
	if (bytestk_count(stack) - count < 0) printf("Error: Stack Underflow.");
#endif

	stack->stacktop -= count;
	*out_arr = stack->stacktop;
}
