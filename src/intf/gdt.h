#pragma once

#include <stdint.h>

#define GDT_NUM_ENTRIES 5
#define GDT_NULL_ENTRY 0
#define GDT_KERNEL_CODE_ENTRY 1
#define GDT_KERNEL_DATA_ENTRY 2
#define GDT_USER_CODE_ENTRY 3
#define GDT_USER_DATA_ENTRY 4

typedef struct {
    uint16_t limit;
    uint64_t address;
} __attribute__((packed)) gdtr_t;

typedef struct {
    uint64_t entries[GDT_NUM_ENTRIES];
    gdtr_t pointer;
} gdt_t;

// GDT management functions
void gdt_initialize(gdt_t* gdt);
void gdt_load(const gdt_t* gdt);
void gdt_flush(void);
