#include <stddef.h>

void* memset(void* ptr, int x, size_t n)
{
	unsigned char* p = (unsigned char*)ptr;

	unsigned char value = (unsigned char)x;

	for (size_t i = 0; i < n; i++)
	{
		p[i] = value;
	}
	return ptr;
}
