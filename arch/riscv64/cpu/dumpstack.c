/* arch/riscv64/cpu/dumpstack.c */

#include <stdint.h>

#include <kernel/panic.h>
#include <lib/kprint.h>
#include <arch/general/dumpstack.h>
#include <arch/riscv64/error_code.h>
#include <arch/riscv64/interrupts.h>

void die(uintptr_t code, pt_regs regs)
{
	// Disable interupts
	
	disable_interrupts();
	panic(code_name(code));
	show_regs(regs);
}

void show_regs(pt_regs regs)
{
	kprintf("ra: %x\tsp: %x\tgp: %x\ttp: %x\n", regs.ra, regs.sp, regs.gp, regs.tp);
	kprintf("t0: %x\tt1: %x\tt2: %x\n", regs.t0, regs.t1, regs.t2);
	kprintf("s0: %x\ts1: %x\n", regs.s0, regs.s1);
	kprintf("a0: %x\ta1: %x\ta2: %x\n", regs.a0, regs.a1, regs.a2);
	kprintf("a3: %x\ta4: %x\ta5: %x\n", regs.a3, regs.a4, regs.a5);
	kprintf("a5: %x\ta6: %x\n", regs.a6, regs.a7);
	kprintf("t3: %x\tt4: %x\tt5: %x\tt6: %x\n", regs.t3, regs.t4, regs.t5, regs.t6);
	kprintf("epc: %x\n", regs.epc);
	kprintf("status: %x\n", regs.status);
	kprintf("badaddr: %x\n", regs.badaddr);
	kprintf("cause: %x\n", regs.cause);
}

static inline unsigned long read_sstatus(void) {
    unsigned long value;
    asm volatile ("csrr %0, sstatus" : "=r"(value));
    return value;
}

pt_regs get_regs(uintptr_t *regs, uintptr_t cause, uintptr_t epc, uintptr_t tval)
{
	pt_regs regs_s;
	regs_s.ra = regs[0];
	regs_s.sp = regs[2];
	regs_s.gp = regs[3];
	regs_s.tp = regs[4];
	regs_s.t0 = regs[5];
	regs_s.t1 = regs[6];
	regs_s.t2 = regs[7];
	regs_s.s0 = regs[8];
	regs_s.s1 = regs[9];
	regs_s.a0 = regs[10];
	regs_s.a1 = regs[11];
	regs_s.a2 = regs[12];
	regs_s.a3 = regs[13];
	regs_s.a4 = regs[14];
	regs_s.a5 = regs[15];
	regs_s.a6 = regs[16];
	regs_s.a7 = regs[17];
	regs_s.t3 = regs[28];
	regs_s.t4 = regs[29];
	regs_s.t5 = regs[30];
	regs_s.t6 = regs[31];
	regs_s.epc = epc;
	regs_s.status = read_sstatus();
	regs_s.badaddr = tval;
	regs_s.cause = cause;
	return regs_s;
}
