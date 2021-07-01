#pragma once

#include "bytecode.h"

void debug_disassemble_bytec(ByteCode* chunk, const char* name);
int debug_disassemble_opcode(ByteCode* chunk, int offset);