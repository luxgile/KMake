#include <stdio.h>
#include "kdebug.h"
#include "runLines.h"
#include "commonTypes.h"

void debug_disassemble_bytec(ByteCode* chunk, const char* name)
{
	printf("== %s ==\n", name);

	for (size_t i = 0; i < chunk->count;)
	{
		i = debug_disassemble_opcode(chunk, i);
	}
}

static int simpleInstruction(const char* name, int offset)
{
	printf("%s\n", name);
	return offset + 1;
}

static int constantInstruction(const char* name, ByteCode* chunk, int offset)
{
	//Get constant parameter
	TYPE_ID type = chunk->code[offset + 1];
	TypeInfo* typeInfo = typetbl_get_info(type);
	Byte1 constant = chunk->code[offset + 2];

	printf("%-16s %4d ", name, constant);
	printf("%u '", typeInfo->size);

	switch (type)
	{
	case TYPEID_DEC: printf("%g", bytearr_read(&chunk->constants, double, constant)); break;
	case TYPEID_BOOL: printf("%s", bytearr_read(&chunk->constants, bool, constant) ? "true" : "false"); break;
	case TYPEID_STRING: printf("%s", (char*)bytearr_read(&chunk->constants, StringPointer*, constant)->base.p); break;
	}

	printf("'\n");
	return offset + 3;
}

int debug_disassemble_opcode(ByteCode* chunk, int offset)
{
	printf("%04d ", offset);

	if (offset > 0 && linearr_get_line(&chunk->lines, offset) == linearr_get_line(&chunk->lines, offset - 1)) printf("\t| ");
	else printf("%4d ", linearr_get_line(&chunk->lines, offset));

	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
	case OP_RETURN: return simpleInstruction("OP_RETURN", offset);
	case OP_PRINT: return simpleInstruction("OP_PRINT", offset);
	case OP_POP: return simpleInstruction("OP_POP", offset);


	case OP_CONSTANT: return constantInstruction("OP_CONSTANT", chunk, offset);
	case OP_DEFINE_GLOBAL: return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);

	case OP_ADD: return simpleInstruction("OP_ADD", chunk, offset);
	case OP_SUBTRACT: return simpleInstruction("OP_SUBSTRACT", chunk, offset);
	case OP_MULT: return simpleInstruction("OP_MULT", chunk, offset);
	case OP_DIVIDE: return simpleInstruction("OP_DIVIDE", chunk, offset);

	case OP_NEGATE: return simpleInstruction("OP_NEGATE", offset);
	case OP_EQUALS: return simpleInstruction("OP_EQUALS", offset);

	case OP_TRUE: return simpleInstruction("OP_TRUE", offset);
	case OP_FALSE: return simpleInstruction("OP_FALSE", offset);

	case OP_NOT: return simpleInstruction("OP_NOT", offset);
	//case OP_IS: return simpleInstruction("OP_IS", offset);

	default:
		printf("Unknown OpCode %d\n", instruction);
		return offset + 1;
	}
}
