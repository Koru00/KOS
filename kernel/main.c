/* kernel/main.c */

#include <lib/kprint.h>
#include <kernel/mm/mm.h>
#include <arch/general/platform.h>

#define DRAM_END 0x88000000

void kernel_init(void);

__attribute__((noreturn))
void kernel_main(void) {
	kprintf("Booting Kernel...\n");
	kprintf("- kernel init:\n");
	
	kernel_init();

	while (1)
	{}
}

void kernel_init(void)
{
	kprintf("[ ] arch_init\r");
	arch_init();
	kprintf("[x] arch_init\n");

	kprintf("[ ] pmm init\r");
	uintptr_t pmm_start = (uintptr_t)_kernel_end;
    	uintptr_t pmm_end   = (uintptr_t)DRAM_END;
	pmm_init(pmm_start, pmm_end);
	kprintf("[x] pmm init\n");
	
	kprintf("[ ] vmm init\r");
	vmm_init();
	kprintf("[x] vmm init\n");
}
