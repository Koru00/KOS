// include/arch/riscv64/mm/mmu.h

#define PTE_V (1 << 0) // Valid
#define PTE_R (1 << 1) // Read
#define PTE_W (1 << 2) // Write
#define PTE_X (1 << 3) // Execute

typedef uint64_t pte_t;

// Standard Sv39 translation
static inline int get_idx(uintptr_t va, int level) {
    return (va >> (12 + (level * 9))) & 0x1FF;
}

// The Professional "Walk": Find (or create) a leaf PTE
pte_t *page_table_walk(pte_t *root, uintptr_t va, int alloc) {
    pte_t *current = root;
    
    for (int level = 2; level > 0; level--) {
        int idx = get_idx(va, level);
        if (!(current[idx] & PTE_V)) {
            if (!alloc) return NULL;
            
            // Level doesn't exist, allocate a new page for the next level
            void *next_lvl = pmm_alloc();
            // Convert physical address to PPN and set Valid bit
            current[idx] = (((uintptr_t)next_lvl >> 12) << 10) | PTE_V;
        }
        // Move to the next level
        current = (pte_t*)(((current[idx] >> 10) << 12));
    }
    return &current[get_idx(va, 0)];
}
