#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "bytecode.h"
#include "typeTable.h"
#include "kdebug.h"
#include "byteArray.h"
#include "vm.h"

static void repl(VM* vm)
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

		vm_interpret(vm, line);
	}
}

int main(int argc, const char* argv[])
{
	VM vm;
	vm_init(&vm);

	repl(&vm);

	vm_free(&vm);
	return 0;
}