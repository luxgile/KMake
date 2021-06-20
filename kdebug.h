#ifndef __KDEBUG_HEADER__
#define __KDEBUG_HEADER__

#include "bytecode.h"

void disassembleChunk(ByteCode* chunk, const char* name);
int disassembleInstruction(ByteCode* chunk, int offset);

#endif