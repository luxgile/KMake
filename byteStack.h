#ifndef __BYTE_STACK_HEADER__
#define __BYTE_STACK_HEADER__

#include "common.h"

typedef struct ByteStack {
	int capacity;
	Byte1* stack;
	Byte1* stacktop;
} ByteStack;

void bytestk_init(ByteStack* stack, int capacity);
void bytestk_free(ByteStack* stack);

void bytestk_push(ByteStack* stack, Byte1 byte);
void bytestk_push_arr(ByteStack* stack, Byte1* arr, int count);

Byte1 bytestk_pop(ByteStack* stack);
void bytestk_pop_arr(ByteStack* stack, Byte1** out_arr, int count);

void bytestk_clear(ByteStack* stack);

#endif