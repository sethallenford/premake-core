#include <stdio.h>

int main(void)
{
	if (sizeof(void*) != 8)
	{
		printf("\x1b[31;1m");
		printf("\n\nERROR: Premake must be compiled in 64-bit.\n");
		printf("Start a 'x64 Native Tools Command Prompt'.\n");
		printf("or run 'vcvarsall.bat x64'.\n\n");
		printf("\x1b[0m");
		return -1;
	}
	return 0;
}
