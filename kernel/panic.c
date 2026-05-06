#include <lib/kprint.h>


void panic(const char* reason)
{
	// Stamp with time from start up
	kprintf("Kernel panic - not syncing:\t%s\n", reason);	
}

void kernel_crash()
{
	// Do stuff before calling arch specific die funct
}
