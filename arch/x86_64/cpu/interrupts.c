#include <stdint.h>

void default_interrupt_handler()
{
    while (1) {
        asm volatile ("hlt");
    }
}

void general_protection_fault()
{
    while (1) {
        asm volatile ("hlt");
    }
}

void double_fault()
{
    while (1) {
        asm volatile ("hlt");
    }
}

void page_fault_handler(uint64_t error_code, uint64_t rip, uint64_t cr2)
{
    // For now just halt safely
    while (1) {
        asm volatile ("hlt");
    }
}
