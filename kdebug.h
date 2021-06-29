#ifndef __KDEBUG_HEADER__
#define __KDEBUG_HEADER__

#include "bytecode.h"

void debug_disassemble_bytec(ByteCode* chunk, const char* name);
int debug_disassemble_opcode(ByteCode* chunk, int offset);

#endif