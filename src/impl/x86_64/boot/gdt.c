#include "gdt.h"

static void create_gdt_entry(uint64_t* entry, uint32_t base, uint32_t limit, 
                            uint8_t type, uint8_t s, uint8_t dpl, uint8_t p, 
                            uint8_t avl, uint8_t l, uint8_t db, uint8_t g) {
    *entry = limit & 0xFFFF;
    *entry |= (base & 0xFFFF) << 16;
    *entry |= (type & 0xF) << 40;
    *entry |= (s & 1) << 44;
    *entry |= (dpl & 3) << 45;
    *entry |= (p & 1) << 47;
    *entry |= ((limit >> 16) & 0xF) << 48;
    *entry |= (avl & 1) << 52;
    *entry |= (l & 1) << 53;
    *entry |= (db & 1) << 54;
    *entry |= (g & 1) << 55;
    *entry |= ((base >> 16) & 0xFF) << 56;
    *entry |= ((base >> 24) & 0xFF) << 56;  // Bug fix: this should be base >> 24
}

void gdt_initialize(gdt_t* gdt) {
    // Initialize pointer
    gdt->pointer.limit = GDT_NUM_ENTRIES * sizeof(uint64_t) - 1;
    gdt->pointer.address = (uint64_t)gdt->entries;
    
    // Null descriptor (required)
    gdt->entries[GDT_NULL_ENTRY] = 0;
    
    // Kernel code segment (64-bit, readable, executable)
    create_gdt_entry(&gdt->entries[GDT_KERNEL_CODE_ENTRY], 0, 0, 
                    0xB, 1, 0, 1, 0, 1, 0, 0);
    
    // Kernel data segment (writable)
    create_gdt_entry(&gdt->entries[GDT_KERNEL_DATA_ENTRY], 0, 0, 
                    0x3, 1, 0, 1, 0, 1, 0, 0);
    
    // User code segment (64-bit, readable, executable, DPL=3)
    create_gdt_entry(&gdt->entries[GDT_USER_CODE_ENTRY], 0, 0, 
                    0xB, 1, 3, 1, 0, 1, 0, 0);
    
    // User data segment (writable, DPL=3)
    create_gdt_entry(&gdt->entries[GDT_USER_DATA_ENTRY], 0, 0, 
                    0x3, 1, 3, 1, 0, 1, 0, 0);
}

void gdt_load(const gdt_t* gdt) {
    gdtr_t pointer = gdt->pointer;
    asm volatile("lgdt %0" : : "m"(pointer));
}

void gdt_flush(void) {
    asm volatile(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "pop %%rdi\n"
        "push $0x8\n"
        "push %%rdi\n"
        "lretq\n"
        : : : "ax", "rdi"
    );
}
