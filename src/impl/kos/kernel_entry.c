#include <stdint.h>

// =============================================================================
// KOS - Kernel Entry Point
// =============================================================================

// Kernel main entry point
void kernel_main(void) {
    // Simple kernel initialization
    while (1) {
        // Basic kernel loop - halt until interrupt
        __asm__ volatile("hlt");
    }
}

// IRQ handler
void irq_handler(uint64_t irq_number) {
    (void)irq_number;
    // TODO: Implement proper IRQ handling
}

// Syscall handler  
void syscall_handler(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    (void)syscall_number;
    (void)arg1;
    (void)arg2;
    (void)arg3;
    // TODO: Implement proper syscall handling
}
