#include <stdint.h>

// =============================================================================
// KOS - Simple Kernel Main Entry Point
// =============================================================================

// Simple kernel main for testing
void kernel_main(void) {
    // Basic kernel initialization
    while (1) {
        // Basic kernel loop
        __asm__ volatile("hlt");
    }
}
