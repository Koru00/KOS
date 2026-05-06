#pragma once

#define MSTATUS_MIE (1 << 3)

static inline void disable_interrupts() 
{
    __asm__ volatile("csrc mstatus, %0" :: "r"(MSTATUS_MIE));
}
