#pragma once
#include <stdint.h>

// C-level handlers
void general_protection_fault();
void page_fault_handler(uint64_t error_code, uint64_t rip, uint64_t cr2);
void double_fault();
void default_interrupt_handler();
