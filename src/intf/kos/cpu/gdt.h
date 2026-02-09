#pragma once

#include "gdt_types.h"
#include "../config.h"

// =============================================================================
// KOS - Global Descriptor Table Interface
// =============================================================================

// GDT management functions
kos_result_t kos_gdt_init(kos_gdt_t* gdt);
void kos_gdt_load(const kos_gdt_t* gdt);
void kos_gdt_flush(void);
kos_result_t kos_gdt_create_entry(kos_gdt_entry_t* entry, u32 base, u32 limit, 
                                  u8 access, u8 flags);

// Convenience functions
kos_result_t kos_gdt_setup_kernel_segments(kos_gdt_t* gdt);
kos_result_t kos_gdt_setup_user_segments(kos_gdt_t* gdt);
