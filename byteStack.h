#pragma once

#include "common.h"

typedef struct {
	int capacity;
	Byte1* stack;
	Byte1* stacktop;
} ByteStack;

void bytestk_init(ByteStack* stack);
void bytestk_free(ByteStack* stack);