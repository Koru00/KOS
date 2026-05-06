/* arch/riscv64/cpu/traps.c */

#include <stdint.h>
#include <kernel/panic.h>
#include <lib/kprint.h>
#include <arch/general/dumpstack.h>

void handle_exception(uintptr_t code, uintptr_t cause, uintptr_t epc, uintptr_t tval, uintptr_t *regs); 
void handle_interrupt(uintptr_t code);

void trap_handler(uintptr_t *stack_ptr) 
{
    	uintptr_t mcause, mepc, mtval;

    	// Read CSRs using inline assembly or helper functions
    	asm volatile("csrr %0, mcause" : "=r"(mcause));
    	asm volatile("csrr %0, mepc"   : "=r"(mepc));
    	asm volatile("csrr %0, mtval"  : "=r"(mtval));

    	// Isolate the exception code (clear the highest bit)
    	uintptr_t code = mcause & 0xfff;
    	int is_interrupt = (mcause >> 63) & 1;

    	if (is_interrupt) 
	{
        	handle_interrupt(code);
	} 
	else 
	{
		handle_exception(code, mcause, mepc, mtval, stack_ptr);
	}
}

void handle_exception(uintptr_t code, uintptr_t cause, uintptr_t epc, uintptr_t tval, uintptr_t *regs) 
{
	pt_regs regs_s = get_regs(regs, cause, epc, tval);	

    	die(code, regs_s);

    	kprint("System Halted.\n");
    	while(1); 
}

void handle_interrupt(uintptr_t code)
{
	// For now just do nothing
}
