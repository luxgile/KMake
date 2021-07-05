#include <stdio.h>
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

		InterpretResult result = vm_interpret(vm, line);
		printf("Interpreted with result: %u\n", result);
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