#include "lib/kprint.h"

void kprint(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		arch_putchar(str[i]);
	}
}
