#ifndef __OPCODE_IMPL_HEADER__
#define __OPCODE_IMPL_HEADER__

#include "opcodes.h"
#include "vm.h"


typedef InterpretResult(*OpCodeFn)(VM* vm);

void opcode_impl_generate(OpCodeRegister* reg);

#endif