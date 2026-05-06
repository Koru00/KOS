/* kernel/main.c */

#include "lib/kprint.h"
#include "kernel/mm/mm.h"

void kernel_main(void) {

	vmm_init();
	
	while (1);
}
