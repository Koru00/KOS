#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Memory Operations Interface (Simple)
// =============================================================================

// Memory types
typedef enum {
    HAL_MEM_TYPE_RAM = 0,
    HAL_MEM_TYPE_ROM = 1,
    HAL_MEM_TYPE_FLASH = 2,
    HAL_MEM_TYPE_CACHE = 3,
    HAL_MEM_TYPE_DEVICE = 4,
    HAL_MEM_TYPE_RESERVED = 5
} hal_memory_type_t;

// Memory region structure
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
} hal_x86_64_memory_region_t;

// Memory operations interface structure
struct hal_memory_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Memory information
    hal_result_t (*get_info)(hal_memory_info_t* info);
    
    // Memory allocation
    void* (*alloc)(hal_size_t size);
    void* (*realloc)(void* ptr, hal_size_t new_size);
    void (*free)(void* ptr);
    void* (*calloc)(hal_size_t count, hal_size_t size);
    
    // Physical memory mapping
    hal_result_t (*map_physical)(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
    hal_result_t (*unmap)(hal_virt_addr_t virt_addr, hal_size_t size);
    
    // Memory protection
    hal_result_t (*protect)(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
    hal_result_t (*unprotect)(hal_virt_addr_t addr, hal_size_t size);
    
    // Memory statistics
    hal_result_t (*get_stats)(hal_memory_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Memory operation convenience functions
hal_result_t hal_memory_init(void);
hal_result_t hal_memory_shutdown(void);
hal_result_t hal_memory_reset(void);

void* hal_memory_alloc(hal_size_t size);
void* hal_memory_realloc(void* ptr, hal_size_t new_size);
void hal_memory_free(void* ptr);
void* hal_memory_calloc(hal_size_t count, hal_size_t size);

hal_result_t hal_memory_map_physical(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
hal_result_t hal_memory_unmap(hal_virt_addr_t virt_addr, hal_size_t size);

hal_result_t hal_memory_protect(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
hal_result_t hal_memory_unprotect(hal_virt_addr_t addr, hal_size_t size);

hal_result_t hal_memory_get_info(hal_memory_info_t* info);
hal_result_t hal_memory_get_stats(hal_memory_stats_t* stats);
hal_result_t hal_memory_reset_stats(void);
hal_result_t hal_memory_dump_stats(void);
