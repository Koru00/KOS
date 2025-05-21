#pragma once

#include <stdint.h>

typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} registers_t;


typedef void (*isr_t)(registers_t *);

void isr_register_handler(int int_no, isr_t handler);
void isr_handler(registers_t *regs);
