#include "kernel/mm/mm.h"
#include <kernel/panic.h>

// Hardware address extraction macros
#define VA_TO_VPN(va, level) (((va) >> (12 + (level) * 9)) & 0x1FF)
#define PA_TO_PPN(pa)        ((pa) >> 12)
#define PTE_TO_PA(pte)       (((pte) >> 10) << 12)

// The root of our kernel page table
pte_t *kernel_root_table;

// Helper: Walk the 3-level page table to find the leaf entry
static pte_t *page_table_walk(pte_t *root, uintptr_t va, int alloc) {
    pte_t *current_table = root;
    
    // Sv39 has 3 levels: 2, 1, 0
    for (int level = 2; level > 0; level--) {
        int vpn = VA_TO_VPN(va, level);
        pte_t *pte = &current_table[vpn];
        
        if (*pte & PTE_V) {
            // Table exists, move down to the next level
            current_table = (pte_t *)PTE_TO_PA(*pte);
        } else {
            // Table is missing
            if (!alloc) return NULL;
            
            // Allocate a new physical page for the next level
            void *new_table = pmm_alloc();
            if (!new_table) return NULL; // Out of memory!
            
            // Link the new table into the current PTE
            *pte = (PA_TO_PPN((uintptr_t)new_table) << 10) | PTE_V;
            current_table = (pte_t *)new_table;
        }
    }
    
    // Return the Level 0 leaf entry
    return &current_table[VA_TO_VPN(va, 0)];
}

// Map a specific Virtual Address to a Physical Address
int map_page(pte_t *root_table, uintptr_t va, uintptr_t pa, int flags) {
    pte_t *pte = page_table_walk(root_table, va, 1);
 
    if (!pte) return -1; // Allocation failed
    
    if (*pte & PTE_V) {
        // Panic! We are trying to remap an already mapped page.
        return -2; 
    }
    
    *pte = (PA_TO_PPN(pa) << 10) | flags | PTE_V;
    return 0;
}

// Map a large block of memory (must be page-aligned)
int map_range(pte_t *root, uintptr_t va, uintptr_t pa, size_t size, int flags) {
    // Align size up to the nearest 4KB
    size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    for (size_t offset = 0; offset < size; offset += PAGE_SIZE) {
        if (map_page(root, va + offset, pa + offset, flags) != 0) {
            return -1; // Failed
        }
    }
    return 0;
}

// The Grand Finale: Activating Virtual Memory
void vmm_init() {
    // 1. Allocate the Root Page Table
    kernel_root_table = (pte_t *)pmm_alloc();
    
    // 2. Map Kernel Code (.text) as Read-Execute ONLY (W^X security)
    size_t text_size = (uintptr_t)_text_end - (uintptr_t)_text_start;
    if (map_range(kernel_root_table, (uintptr_t)_text_start, (uintptr_t)_text_start, text_size, PTE_R | PTE_X) != 0)
    {
	panic("VMM init: failed to map kernel code\n");
    }
    
    // 3. Map Kernel Data (.data, .bss) as Read-Write ONLY
    size_t data_size = (uintptr_t)_kernel_end - (uintptr_t)_data_start;
    map_range(kernel_root_table, (uintptr_t)_data_start, (uintptr_t)_data_start, data_size, PTE_R | PTE_W);
    
    // 4. Map Memory Mapped I/O (MMIO) - e.g., the UART
    // Assuming QEMU virt UART is at 0x10000000 and needs 1 page
    map_page(kernel_root_table, 0x10000000, 0x10000000, PTE_R | PTE_W);

    // 5. Build the satp register value: Mode Sv39 (8) | PPN
    uintptr_t satp = (8ULL << 60) | PA_TO_PPN((uintptr_t)kernel_root_table);
    
    // 6. Switch on the MMU!
    asm volatile("csrw satp, %0" :: "r"(satp));
    asm volatile("sfence.vma zero, zero"); // Flush the Translation Lookaside Buffer
}
