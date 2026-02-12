#pragma once

#include <stdint.h>
#include "gdt.h"

// Kernel initialization phases
typedef enum {
    KERNEL_INIT_PHASE_EARLY,     // GDT, basic CPU setup
    KERNEL_INIT_PHASE_INTERRUPTS, // IDT, PIC, IRQ
    KERNEL_INIT_PHASE_DRIVERS,   // VGA, keyboard, timer
    KERNEL_INIT_PHASE_MEMORY,    // Heap, memory management
    KERNEL_INIT_PHASE_SERVICES,  // Shell, filesystem
    KERNEL_INIT_PHASE_COMPLETE
} kernel_init_phase_t;

// Kernel state
typedef struct {
    gdt_t gdt;
    kernel_init_phase_t current_phase;
    uint32_t tick_count;
} kernel_state_t;

// Main kernel functions
void kernel_main(void);
void kernel_initialize(void);
void kernel_run(void);

// Phase-specific initialization
void kernel_initialize_early_setup(void);
void kernel_initialize_interrupts(void);
void kernel_initialize_drivers(void);
void kernel_initialize_memory(void);
void kernel_initialize_services(void);

// Utility functions
const char* kernel_get_version(void);
kernel_state_t* kernel_get_state(void);
