#include <stdint.h>

// =============================================================================
// KOS - Stub Functions for Missing Dependencies
// =============================================================================

// Kernel main entry point stub
extern void kernel_main(void);

// IRQ handler stub
void irq_handler(uint64_t irq_number) {
    (void)irq_number;
    // TODO: Implement proper IRQ handling
}

// Syscall handler stub
void syscall_handler(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    (void)syscall_number;
    (void)arg1;
    (void)arg2;
    (void)arg3;
    // TODO: Implement proper syscall handling
}
