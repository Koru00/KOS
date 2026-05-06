#pragma once

#include <stdint.h>

typedef struct pt_regs {
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0, t1, t2;
    unsigned long s0, s1;
    unsigned long a0, a1, a2, a3, a4, a5, a6, a7;
    unsigned long s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    unsigned long t3, t4, t5, t6;
    unsigned long epc;
    unsigned long status;
    unsigned long badaddr;
    unsigned long cause;
} pt_regs;

void die(uintptr_t code, pt_regs regs);
void show_regs(pt_regs regs);
pt_regs get_regs(uintptr_t *regs, uintptr_t cause, uintptr_t epc, uintptr_t tval);
