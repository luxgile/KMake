#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "bytecode.h"
#include "typeTable.h"
#include "kdebug.h"
#include "byteArray.h"
#include "vm.h"

static void repl()
{
	char line[1024];
	for (;;)
	{
		printf(">");
		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}

		interpret(line);
	}
}

int main(int argc, const char* argv[])
{
	initVM();
	TypeTable_Init();

	repl();

	freeVM();
	TypeTable_Free();
	return 0;
}