#pragma once

#include <stdint.h>
#include "../types.h"

// =============================================================================
// KOS - GDT Type Definitions (standalone)
// =============================================================================

// Basic types needed for GDT
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// GDT entry indices
typedef enum {
    KOS_GDT_NULL = 0,
    KOS_GDT_KERNEL_CODE = 1,
    KOS_GDT_KERNEL_DATA = 2,
    KOS_GDT_USER_CODE = 3,
    KOS_GDT_USER_DATA = 4,
    KOS_GDT_MAX_ENTRIES = 5
} kos_gdt_index_t;

// GDT access rights
typedef enum {
    KOS_GDT_ACCESS_CODE_READABLE = 0x1,
    KOS_GDT_ACCESS_DATA_WRITABLE = 0x1,
    KOS_GDT_ACCESS_CODE_CONFORMING = 0x4,
    KOS_GDT_ACCESS_SYSTEM = 0x0,
    KOS_GDT_ACCESS_TSS = 0x9
} kos_gdt_access_t;

// GDT flags
typedef enum {
    KOS_GDT_FLAG_PRESENT = 0x80,
    KOS_GDT_FLAG_PRIVILEGE_SHIFT = 5,
    KOS_GDT_FLAG_LONG_MODE = 0x20,
    KOS_GDT_FLAG_SIZE_32BIT = 0x4,
    KOS_GDT_FLAG_GRANULARITY_4K = 0x8
} kos_gdt_flag_t;

// GDT entry structure (64-bit)
typedef struct {
    u64 raw;
} kos_gdt_entry_t;

// GDT pointer structure
typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) kos_gdt_pointer_t;

// Complete GDT structure
typedef struct {
    kos_gdt_entry_t entries[KOS_GDT_MAX_ENTRIES];
    kos_gdt_pointer_t pointer;
} kos_gdt_t;
