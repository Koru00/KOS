#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_memory_simple.h"
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "debug/debug.h"

// =============================================================================
// KOS - HAL Memory Operations x86_64 Implementation
// =============================================================================

// Forward declarations
extern struct memory_manager g_memory_manager;

// x86_64-specific memory information
typedef struct {
    hal_u64_t total_memory;
    hal_u64_t available_memory;
    hal_u32_t page_size;
    hal_u32_t total_pages;
    hal_u32_t free_pages;
    hal_u32_t used_pages;
    hal_bool_t paging_enabled;
    hal_bool_t protection_enabled;
    hal_bool_t nx_enabled;
    hal_bool_t pae_enabled;
    hal_u32_t physical_address_width;
    hal_u32_t linear_address_width;
    hal_virt_addr_t kernel_base;
    hal_virt_addr_t kernel_end;
    hal_virt_addr_t user_base;
    hal_virt_addr_t user_end;
    hal_virt_addr_t direct_map_base;
    hal_virt_addr_t direct_map_end;
} hal_x86_64_memory_info_t;

// Memory region structure for x86_64
typedef struct {
    hal_virt_addr_t start;
    hal_virt_addr_t end;
    hal_phys_addr_t phys_addr;
    hal_size_t size;
    hal_memory_type_t type;
    hal_memory_prot_t protection;
    hal_bool_t is_mapped;
    hal_bool_t is_reserved;
    const char* name;
    struct hal_x86_64_memory_region* next;
} hal_x86_64_memory_region_local_t;

// Global x86_64 memory state
static hal_x86_64_memory_info_t g_x86_64_memory_info = {0};
static hal_memory_info_t g_memory_info = {0};
static hal_x86_64_memory_region_local_t* g_memory_regions = NULL;
static bool g_memory_initialized = false;

// x86_64 memory operations implementation
static hal_result_t hal_x86_64_memory_init(void);
static hal_result_t hal_x86_64_memory_shutdown(void);
static hal_result_t hal_x86_64_memory_reset(void);
static hal_result_t hal_x86_64_memory_cleanup(void);
static hal_result_t hal_x86_64_memory_get_info(hal_memory_info_t* info);
static void* hal_x86_64_memory_alloc(hal_size_t size);
static void hal_x86_64_memory_free(void* ptr);
static hal_result_t hal_x86_64_memory_map_physical(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
static hal_result_t hal_x86_64_memory_unmap(hal_virt_addr_t virt_addr, hal_size_t size);
static hal_result_t hal_x86_64_memory_protect(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
static hal_result_t hal_x86_64_memory_get_stats(hal_memory_stats_t* stats);

// x86_64 memory operations structure
static hal_memory_ops_t hal_x86_64_memory_ops = {
    .init = hal_x86_64_memory_init,
    .shutdown = hal_x86_64_memory_shutdown,
    .reset = hal_x86_64_memory_reset,
    .cleanup = hal_x86_64_memory_cleanup,
    .get_info = hal_x86_64_memory_get_info,
    .alloc = hal_x86_64_memory_alloc,
    .free = hal_x86_64_memory_free,
    .map_physical = hal_x86_64_memory_map_physical,
    .unmap = hal_x86_64_memory_unmap,
    .protect = hal_x86_64_memory_protect,
    .get_stats = hal_x86_64_memory_get_stats
};

// Page table entry structure
typedef struct {
    hal_u64_t present : 1;
    hal_u64_t read_write : 1;
    hal_u64_t user_supervisor : 1;
    hal_u64_t write_through : 1;
    hal_u64_t cache_disable : 1;
    hal_u64_t accessed : 1;
    hal_u64_t dirty : 1;
    hal_u64_t pat : 1;
    hal_u64_t global : 1;
    hal_u64_t available1 : 3;
    hal_u64_t frame : 40;
    hal_u64_t available2 : 11;
    hal_u64_t nx : 1;
} hal_x86_64_pte_t;

// Control register access
static inline hal_u64_t hal_x86_64_read_cr0(void) {
    hal_u64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r" (cr0));
    return cr0;
}

static inline hal_u64_t hal_x86_64_read_cr3(void) {
    hal_u64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    return cr3;
}

static inline hal_u64_t hal_x86_64_read_cr4(void) {
    hal_u64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r" (cr4));
    return cr4;
}

static inline void hal_x86_64_write_cr3(hal_u64_t cr3) {
    asm volatile("mov %0, %%cr3" : : "r" (cr3));
}

// TLB flush
static inline void hal_x86_64_flush_tlb(void) {
    hal_x86_64_write_cr3(hal_x86_64_read_cr3());
}

// Get page size
static hal_u32_t hal_x86_64_get_page_size(void) {
    return 4096; // Standard 4KB pages
}

// Get total memory (simplified)
static hal_u64_t hal_x86_64_get_total_memory(void) {
    // In a real implementation, this would scan the memory map
    // For now, we assume 1GB of total memory
    return 1024 * 1024 * 1024ULL;
}

// Initialize memory regions
static void hal_x86_64_init_memory_regions(void) {
    // Clear existing regions
    g_memory_regions = NULL;
    
    // Set up address space layout
    g_x86_64_memory_info.kernel_base = (hal_virt_addr_t)0xFFFF800000000000ULL;
    g_x86_64_memory_info.kernel_end = (hal_virt_addr_t)0xFFFFFFFFFFFFFFFFULL;
    g_x86_64_memory_info.user_base = (hal_virt_addr_t)0x0000000000000000ULL;
    g_x86_64_memory_info.user_end = (hal_virt_addr_t)0x00007FFFFFFFFFFFULL;
    g_x86_64_memory_info.direct_map_base = (hal_virt_addr_t)0xFFFF880000000000ULL;
    g_x86_64_memory_info.direct_map_end = (hal_virt_addr_t)0xFFFFFFFFFFFFFFFFULL;
    
    // Add kernel region
    hal_x86_64_memory_region_local_t* kernel_region = (hal_x86_64_memory_region_local_t*)kos_memory_alloc(&g_memory_manager, sizeof(hal_x86_64_memory_region_local_t));
    if (kernel_region) {
        kernel_region->start = g_x86_64_memory_info.kernel_base;
        kernel_region->end = g_x86_64_memory_info.kernel_end;
        kernel_region->size = kernel_region->end - kernel_region->start;
        kernel_region->type = HAL_MEM_TYPE_RAM;
        kernel_region->protection = HAL_MEM_PROT_READ_WRITE_EXECUTE;
        kernel_region->is_mapped = true;
        kernel_region->is_reserved = true;
        kernel_region->name = "Kernel Space";
        kernel_region->next = (struct hal_x86_64_memory_region*)g_memory_regions;
        g_memory_regions = kernel_region;
    }
    
    // Add user region
    hal_x86_64_memory_region_local_t* user_region = (hal_x86_64_memory_region_local_t*)kos_memory_alloc(&g_memory_manager, sizeof(hal_x86_64_memory_region_local_t));
    if (user_region) {
        user_region->start = g_x86_64_memory_info.user_base;
        user_region->end = g_x86_64_memory_info.user_end;
        user_region->size = user_region->end - user_region->start;
        user_region->type = HAL_MEM_TYPE_RAM;
        user_region->protection = HAL_MEM_PROT_READ_WRITE_EXECUTE;
        user_region->is_mapped = true;
        user_region->is_reserved = false;
        user_region->name = "User Space";
        user_region->next = (struct hal_x86_64_memory_region*)g_memory_regions;
        g_memory_regions = user_region;
    }
    
    // Add direct map region
    hal_x86_64_memory_region_local_t* direct_region = (hal_x86_64_memory_region_local_t*)kos_memory_alloc(&g_memory_manager, sizeof(hal_x86_64_memory_region_local_t));
    if (direct_region) {
        direct_region->start = g_x86_64_memory_info.direct_map_base;
        direct_region->end = g_x86_64_memory_info.direct_map_end;
        direct_region->size = direct_region->end - direct_region->start;
        direct_region->type = HAL_MEM_TYPE_RAM;
        direct_region->protection = HAL_MEM_PROT_READ_WRITE;
        direct_region->is_mapped = true;
        direct_region->is_reserved = true;
        direct_region->name = "Direct Map";
        direct_region->next = (struct hal_x86_64_memory_region*)g_memory_regions;
        g_memory_regions = direct_region;
    }
}

// Memory initialization
static hal_result_t hal_x86_64_memory_init(void) {
    if (g_memory_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Initializing x86_64 memory operations");
    
    // Initialize memory manager if not already done
    kos_result_t result = kos_memory_init(&g_memory_manager);
    if (result != KOS_SUCCESS) {
        log_error("Failed to initialize memory manager");
        return HAL_ERROR_HARDWARE;
    }
    
    // Get basic memory information
    g_x86_64_memory_info.page_size = hal_x86_64_get_page_size();
    g_x86_64_memory_info.total_memory = hal_x86_64_get_total_memory();
    g_x86_64_memory_info.available_memory = g_x86_64_memory_info.total_memory;
    g_x86_64_memory_info.total_pages = g_x86_64_memory_info.total_memory / g_x86_64_memory_info.page_size;
    g_x86_64_memory_info.free_pages = g_x86_64_memory_info.total_pages;
    g_x86_64_memory_info.used_pages = 0;
    
    // Check paging status
    hal_u64_t cr0 = hal_x86_64_read_cr0();
    g_x86_64_memory_info.paging_enabled = (cr0 & (1 << 31)) != 0;
    
    // Check protection status
    g_x86_64_memory_info.protection_enabled = true; // Assume protection is enabled
    
    // Check NX status
    hal_u64_t efer;
    asm volatile("rdmsr" : "=A" (efer) : "c" (0xC0000080));
    g_x86_64_memory_info.nx_enabled = (efer & (1 << 11)) != 0;
    
    // Check PAE status
    hal_u64_t cr4 = hal_x86_64_read_cr4();
    g_x86_64_memory_info.pae_enabled = (cr4 & (1 << 5)) != 0;
    
    // Get address widths
    g_x86_64_memory_info.physical_address_width = 48; // Simplified
    g_x86_64_memory_info.linear_address_width = 48;    // Simplified
    
    // Initialize memory regions
    hal_x86_64_init_memory_regions();
    
    // Initialize memory info structure
    kos_memset(&g_memory_info, 0, sizeof(hal_memory_info_t));
    g_memory_info.total_memory = g_x86_64_memory_info.total_memory;
    g_memory_info.available_memory = g_x86_64_memory_info.available_memory;
    g_memory_info.page_size = g_x86_64_memory_info.page_size;
    g_memory_info.page_count = g_x86_64_memory_info.total_pages;
    g_memory_info.free_pages = g_x86_64_memory_info.free_pages;
    g_memory_info.used_pages = g_x86_64_memory_info.used_pages;
    g_memory_info.has_paging = g_x86_64_memory_info.paging_enabled;
    g_memory_info.has_protection = g_x86_64_memory_info.protection_enabled;
    g_memory_info.has_nx = g_x86_64_memory_info.nx_enabled;
    g_memory_info.has_pae = g_x86_64_memory_info.pae_enabled;
    
    g_memory_initialized = true;
    
    log_info("x86_64 memory operations initialized");
    log_info("Total Memory: %llu MB", g_x86_64_memory_info.total_memory / HAL_MB);
    log_info("Page Size: %u KB", g_x86_64_memory_info.page_size / HAL_KB);
    log_info("Total Pages: %u", g_x86_64_memory_info.total_pages);
    log_info("Paging: %s", g_x86_64_memory_info.paging_enabled ? "Enabled" : "Disabled");
    log_info("NX: %s", g_x86_64_memory_info.nx_enabled ? "Enabled" : "Disabled");
    
    return HAL_SUCCESS;
}

// Memory shutdown
static hal_result_t hal_x86_64_memory_shutdown(void) {
    if (!g_memory_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    // Free memory regions
    hal_x86_64_memory_region_local_t* region = g_memory_regions;
    while (region) {
        hal_x86_64_memory_region_local_t* next = (hal_x86_64_memory_region_local_t*)region->next;
        kos_memory_free(&g_memory_manager, region);
        region = next;
    }
    g_memory_regions = NULL;
    
    g_memory_initialized = false;
    log_info("x86_64 memory operations shutdown");
    return HAL_SUCCESS;
}

// Memory reset
static hal_result_t hal_x86_64_memory_reset(void) {
    if (!g_memory_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    hal_x86_64_memory_shutdown();
    return hal_x86_64_memory_init();
}

// Memory cleanup
static hal_result_t hal_x86_64_memory_cleanup(void) {
    return hal_x86_64_memory_shutdown();
}

// Get memory information
static hal_result_t hal_x86_64_memory_get_info(hal_memory_info_t* info) {
    if (!g_memory_initialized || !info) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *info = g_memory_info;
    return HAL_SUCCESS;
}

// Allocate memory
static void* hal_x86_64_memory_alloc(hal_size_t size) {
    if (!g_memory_initialized || size == 0) {
        return NULL;
    }
    
    void* ptr = kos_memory_alloc(&g_memory_manager, size);
    if (ptr) {
        g_x86_64_memory_info.available_memory -= size;
        g_x86_64_memory_info.used_pages += (size + g_x86_64_memory_info.page_size - 1) / g_x86_64_memory_info.page_size;
        g_x86_64_memory_info.free_pages = g_x86_64_memory_info.total_pages - g_x86_64_memory_info.used_pages;
    }
    
    return ptr;
}

// Free memory
static void hal_x86_64_memory_free(void* ptr) {
    if (!g_memory_initialized || !ptr) {
        return;
    }
    
    // In a real implementation, we'd need to track the size of allocated blocks
    // For now, we'll just free the pointer
    kos_memory_free(&g_memory_manager, ptr);
}

// Map physical memory
static hal_result_t hal_x86_64_memory_map_physical(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr) {
    if (!g_memory_initialized || !virt_addr || size == 0) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Check alignment
    if (!HAL_IS_ALIGNED(phys_addr, g_x86_64_memory_info.page_size) || 
        !HAL_IS_ALIGNED(size, g_x86_64_memory_info.page_size)) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // In a real implementation, this would involve page table manipulation
    // For now, we'll use the direct map if available
    if (g_x86_64_memory_info.direct_map_base != 0) {
        *virt_addr = (hal_virt_addr_t)((uintptr_t)g_x86_64_memory_info.direct_map_base + (uintptr_t)phys_addr);
        return HAL_SUCCESS;
    }
    
    // Fallback: return error
    return HAL_ERROR_NOT_SUPPORTED;
}

// Unmap memory
static hal_result_t hal_x86_64_memory_unmap(hal_virt_addr_t virt_addr, hal_size_t size) {
    if (!g_memory_initialized || size == 0) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Check alignment
    if (!HAL_IS_ALIGNED(virt_addr, g_x86_64_memory_info.page_size) || 
        !HAL_IS_ALIGNED(size, g_x86_64_memory_info.page_size)) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // In a real implementation, this would involve page table manipulation
    // For now, we'll just return success
    return HAL_SUCCESS;
}

// Protect memory
static hal_result_t hal_x86_64_memory_protect(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection) {
    if (!g_memory_initialized || size == 0) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Check alignment
    if (!HAL_IS_ALIGNED(addr, g_x86_64_memory_info.page_size) || 
        !HAL_IS_ALIGNED(size, g_x86_64_memory_info.page_size)) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // In a real implementation, this would involve page table manipulation
    // For now, we'll just return success
    return HAL_SUCCESS;
}

// Get memory statistics
static hal_result_t hal_x86_64_memory_get_stats(hal_memory_stats_t* stats) {
    if (!g_memory_initialized || !stats) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    kos_memset(stats, 0, sizeof(hal_memory_stats_t));
    stats->total_memory = g_x86_64_memory_info.total_memory;
    stats->available_memory = g_x86_64_memory_info.available_memory;
    stats->used_memory = g_x86_64_memory_info.total_memory - g_x86_64_memory_info.available_memory;
    stats->free_memory = g_x86_64_memory_info.available_memory;
    
    return HAL_SUCCESS;
}

// Get x86_64 memory interface
hal_interface_t* hal_x86_64_get_memory_interface(void) {
    static hal_interface_t interface = {0};
    
    interface.arch_name = "x86_64";
    interface.arch_bits = 64;
    interface.architecture = HAL_ARCH_X86_64;
    interface.memory_ops = &hal_x86_64_memory_ops;
    
    return &interface;
}
