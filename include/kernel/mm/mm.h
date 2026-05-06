#ifndef KERNEL_MM_H
#define KERNEL_MM_H

#include <stdint.h>
#include <stddef.h>

// --- Architecture Constants (RISC-V Sv39) ---
#define PAGE_SIZE 4096
#define PAGE_ORDER 12

// Page Table Entry Flags
#define PTE_V (1 << 0) // Valid
#define PTE_R (1 << 1) // Read
#define PTE_W (1 << 2) // Write
#define PTE_X (1 << 3) // Execute
#define PTE_U (1 << 4) // User

typedef uint64_t pte_t;

// --- Physical Memory Manager (PMM) ---
void  pmm_init(uintptr_t ram_start, uintptr_t ram_end);
void* pmm_alloc(void);
void  pmm_free(void* page);

// --- Virtual Memory Manager (VMM) ---
// Initialize the MMU and switch to virtual addressing
void vmm_init(void);

// Map a single 4KB page
int map_page(pte_t *root_table, uintptr_t va, uintptr_t pa, int flags);

// Map a contiguous range of memory
int map_range(pte_t *root_table, uintptr_t va_start, uintptr_t pa_start, size_t size, int flags);

// --- Kernel Boundaries (Provided by Linker) ---
extern char _text_start[], _text_end[];
extern char _data_start[], _data_end[];
extern char _kernel_end[];

#endif // KERNEL_MM_H
