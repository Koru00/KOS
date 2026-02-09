#include <stdint.h>
#include <string.h>

// =============================================================================
// KOS - Global Descriptor Table Implementation (Standalone)
// =============================================================================

// Basic types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum {
    KOS_SUCCESS = 0,
    KOS_ERROR_INVALID_PARAM = -1
} kos_result_t;

// GDT constants
#define KOS_GDT_MAX_ENTRIES 5
#define KOS_GDT_KERNEL_CODE 1
#define KOS_GDT_KERNEL_DATA 2
#define KOS_GDT_USER_CODE 3
#define KOS_GDT_USER_DATA 4

// GDT flags
#define KOS_GDT_FLAG_PRESENT 0x80
#define KOS_GDT_FLAG_LONG_MODE 0x20
#define KOS_GDT_FLAG_SIZE_32BIT 0x4

// GDT access rights
#define KOS_GDT_ACCESS_CODE_READABLE 0x1
#define KOS_GDT_ACCESS_DATA_WRITABLE 0x1

// GDT structures
typedef struct {
    u64 raw;
} kos_gdt_entry_t;

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) kos_gdt_pointer_t;

typedef struct {
    kos_gdt_entry_t entries[KOS_GDT_MAX_ENTRIES];
    kos_gdt_pointer_t pointer;
} kos_gdt_t;

// GDT functions
kos_result_t kos_gdt_create_entry(kos_gdt_entry_t* entry, u32 base, u32 limit, u8 access, u8 flags) {
    if (!entry) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    u64 raw = 0;
    
    // Pack the GDT entry
    raw |= (limit & 0xFFFF);
    raw |= (base & 0xFFFF) << 16;
    raw |= ((u64)access & 0xFF) << 40;
    raw |= ((u64)flags & 0xF) << 52;
    raw |= ((u64)(limit >> 16) & 0xF) << 48;
    raw |= ((u64)(base >> 16) & 0xFF) << 56;
    raw |= ((u64)(base >> 24) & 0xFF) << 56;
    
    entry->raw = raw;
    return KOS_SUCCESS;
}

kos_result_t kos_gdt_init(kos_gdt_t* gdt) {
    if (!gdt) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Initialize pointer
    gdt->pointer.limit = KOS_GDT_MAX_ENTRIES * sizeof(kos_gdt_entry_t) - 1;
    gdt->pointer.base = (u64)gdt->entries;
    
    // Clear all entries
    for (int i = 0; i < KOS_GDT_MAX_ENTRIES; i++) {
        gdt->entries[i].raw = 0;
    }
    
    return KOS_SUCCESS;
}

kos_result_t kos_gdt_setup_kernel_segments(kos_gdt_t* gdt) {
    if (!gdt) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Kernel code segment (64-bit, readable, executable)
    kos_result_t result = kos_gdt_create_entry(&gdt->entries[KOS_GDT_KERNEL_CODE], 
                                               0, 0, 
                                               KOS_GDT_ACCESS_CODE_READABLE | KOS_GDT_FLAG_PRESENT,
                                               KOS_GDT_FLAG_LONG_MODE);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    // Kernel data segment (writable)
    result = kos_gdt_create_entry(&gdt->entries[KOS_GDT_KERNEL_DATA], 
                                 0, 0, 
                                 KOS_GDT_ACCESS_DATA_WRITABLE | KOS_GDT_FLAG_PRESENT,
                                 KOS_GDT_FLAG_SIZE_32BIT);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    return KOS_SUCCESS;
}

kos_result_t kos_gdt_setup_user_segments(kos_gdt_t* gdt) {
    if (!gdt) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // User code segment (64-bit, readable, executable, DPL=3)
    kos_result_t result = kos_gdt_create_entry(&gdt->entries[KOS_GDT_USER_CODE], 
                                               0, 0, 
                                               KOS_GDT_ACCESS_CODE_READABLE | KOS_GDT_FLAG_PRESENT | (3 << 5),
                                               KOS_GDT_FLAG_LONG_MODE);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    // User data segment (writable, DPL=3)
    result = kos_gdt_create_entry(&gdt->entries[KOS_GDT_USER_DATA], 
                                 0, 0, 
                                 KOS_GDT_ACCESS_DATA_WRITABLE | KOS_GDT_FLAG_PRESENT | (3 << 5),
                                 KOS_GDT_FLAG_SIZE_32BIT);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    return KOS_SUCCESS;
}

void kos_gdt_load(const kos_gdt_t* gdt) {
    if (!gdt) {
        return;
    }
    
    kos_gdt_pointer_t pointer = gdt->pointer;
    asm volatile("lgdt %0" : : "m"(pointer));
}

void kos_gdt_flush(void) {
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
