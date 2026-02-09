#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Memory Operations Interface
// =============================================================================

// Memory protection flags
typedef enum {
    HAL_MEM_PROT_NONE = 0x0,
    HAL_MEM_PROT_READ = 0x1,
    HAL_MEM_PROT_WRITE = 0x2,
    HAL_MEM_PROT_EXECUTE = 0x4,
    HAL_MEM_PROT_READ_WRITE = HAL_MEM_PROT_READ | HAL_MEM_PROT_WRITE,
    HAL_MEM_PROT_READ_EXECUTE = HAL_MEM_PROT_READ | HAL_MEM_PROT_EXECUTE,
    HAL_MEM_PROT_READ_WRITE_EXECUTE = HAL_MEM_PROT_READ | HAL_MEM_PROT_WRITE | HAL_MEM_PROT_EXECUTE
} hal_memory_prot_t;

// Memory types
typedef enum {
    HAL_MEM_TYPE_RAM = 0,
    HAL_MEM_TYPE_ROM = 1,
    HAL_MEM_TYPE_FLASH = 2,
    HAL_MEM_TYPE_CACHE = 3,
    HAL_MEM_TYPE_DEVICE = 4,
    HAL_MEM_TYPE_RESERVED = 5
} hal_memory_type_t;

// Memory mapping types
typedef enum {
    HAL_MAP_TYPE_READ = 0x1,
    HAL_MAP_TYPE_WRITE = 0x2,
    HAL_MAP_TYPE_READ_WRITE = HAL_MAP_TYPE_READ | HAL_MAP_TYPE_WRITE,
    HAL_MAP_TYPE_EXECUTE = 0x4,
    HAL_MAP_TYPE_READ_EXECUTE = HAL_MAP_TYPE_READ | HAL_MAP_TYPE_EXECUTE,
    HAL_MAP_TYPE_READ_WRITE_EXECUTE = HAL_MAP_TYPE_READ | HAL_MAP_TYPE_WRITE | HAL_MAP_TYPE_EXECUTE
} hal_map_type_t;

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
} hal_memory_region_t;

// Memory block structure
typedef struct {
    hal_virt_addr_t addr;
    hal_size_t size;
    hal_bool_t is_free;
    hal_u32_t magic;
    hal_u32_t flags;
    hal_u32_t alloc_id;
    hal_u64_t alloc_time;
    hal_u64_t free_time;
    struct hal_memory_block* next;
    struct hal_memory_block* prev;
} hal_memory_block_t;

// Memory statistics
typedef struct {
    hal_size_t total_memory;
    hal_size_t available_memory;
    hal_size_t used_memory;
    hal_size_t free_memory;
    hal_u32_t total_blocks;
    hal_u32_t used_blocks;
    hal_u32_t free_blocks;
    hal_u32_t fragmentation_count;
    hal_u64_t total_allocations;
    hal_u64_t total_frees;
    hal_u64_t total_alloc_time;
    hal_u64_t total_free_time;
    hal_u64_t max_alloc_time;
    hal_u64_t max_free_time;
    hal_u64_t min_alloc_time;
    hal_u64_t min_free_time;
    hal_u32_t alloc_count;
    hal_u32_t free_count;
    hal_u32_t fail_count;
} hal_memory_stats_t;

// Memory operations interface structure
struct hal_memory_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Memory information
    hal_result_t (*get_info)(hal_memory_info_t* info);
    hal_size_t (*get_total_memory)(hal_size_t* total);
    hal_size_t (*get_available_memory)(hal_size_t* available);
    hal_size_t (*get_used_memory)(hal_size_t* used);
    hal_size_t (*get_free_memory)(hal_size_t* free);
    
    // Memory allocation
    void* (*alloc)(hal_size_t size);
    void* (*realloc)(void* ptr, hal_size_t new_size);
    void (*free)(void* ptr);
    void* (*calloc)(hal_size_t count, hal_size_t size);
    void* (*aligned_alloc)(hal_size_t size, hal_size_t alignment);
    void (*aligned_free)(void* ptr);
    
    // Physical memory mapping
    hal_result_t (*map_physical)(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
    hal_result_t (*unmap)(hal_virt_addr_t virt_addr, hal_size_t size);
    hal_result_t (*remap)(hal_virt_addr_t old_addr, hal_size_t old_size, hal_virt_addr_t* new_addr);
    
    // Memory protection
    hal_result_t (*protect)(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
    hal_result_t (*unprotect)(hal_virt_addr_t addr, hal_size_t size);
    hal_result_t (*get_protection)(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t* protection);
    
    // Memory regions
    hal_result_t (*add_region)(hal_memory_region_t* region);
    hal_result_t (*remove_region)(hal_virt_addr_t start, hal_size_t size);
    hal_result_t (*get_regions)(hal_memory_region_t* regions, hal_u32_t* count);
    hal_result_t (*find_region)(hal_virt_addr_t addr, hal_memory_region_t* region);
    
    // Memory testing
    hal_result_t (*test_memory)(hal_virt_addr_t addr, hal_size_t size);
    hal_result_t (*test_pattern)(hal_virt_addr_t addr, const hal_u8_t* pattern, hal_size_t size);
    hal_result_t (*fill_memory)(hal_virt_addr_t addr, hal_u8_t value, hal_size_t size);
    hal_result_t (*zero_memory)(hal_virt_addr_t addr, hal_size_t size);
    hal_result_t (*copy_memory)(hal_virt_addr_t dest, const void* src, hal_size_t size);
    hal_result_t (*move_memory)(hal_virt_addr_t dest, const void* src, hal_size_t size);
    hal_result_t (*compare_memory)(const void* ptr1, const void* ptr2, hal_size_t size);
    
    // Memory statistics
    hal_result_t (*get_stats)(hal_memory_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
    
    // Memory debugging
    hal_result_t (*dump_memory_map)(void);
    hal_result_t (*dump_memory_regions)(void);
    hal_result_t (*dump_memory_blocks)(void);
    hal_result_t (*validate_memory)(void);
    
    // Advanced features
    hal_result_t (*enable_paging)(void);
    hal_result_t (*disable_paging)(void);
    hal_result_t (*is_paging_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_page_size)(hal_size_t* page_size);
    hal_result_t (*get_total_pages)(hal_u32_t* total_pages);
    hal_result_t (*get_free_pages)(hal_u32_t* free_pages);
    hal_result_t (*get_used_pages)(hal_u32_t* used_pages);
    
    // Cache management
    hal_result_t (*flush_cache)(void);
    hal_result_t (*invalidate_cache)(void);
    hal_result_t (*write_back_cache)(void);
    hal_result_t (*enable_cache)(void);
    hal_result_t (*disable_cache)(void);
    hal_result_t (*is_cache_enabled)(hal_bool_t* enabled);
    
    // NUMA support
    hal_result_t (*enable_numa)(void);
    hal_result_t (*disable_numa)(void);
    hal_result_t (*is_numa_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_numa_state)(hal_u32_t* state);
    
    // Huge pages
    hal_result_t (*enable_huge_pages)(void);
    hal_result_t (*disable_huge_pages)(void);
    hal_result_t (*is_huge_pages_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_huge_page_size)(hal_size_t* size);
    
    // Address space layout
    hal_result_t (*get_kernel_space)(hal_virt_addr_t* start, hal_virt_addr_t* end);
    hal_result_t (*get_user_space)(hal_virt_addr_t* start, hal_virt_addr_t* end);
    hal_result_t (*get_direct_map_range)(hal_virt_addr_t* start, hal_virt_addr_t* end);
    
    // Memory barriers
    hal_result_t (*memory_barrier)(void);
    hal_result_t (*read_barrier)(void);
    hal_result_t (*write_barrier)(void);
    hal_result_t (*compiler_barrier)(void);
    hal_result_t (*instruction_barrier)(void);
    
    // Memory hot-plug support
    hal_result_t (*enable_hotplug)(void);
    hal_result_t (*disable_hotplug)(void);
    hal_result_t (*is_hotplug_enabled)(hal_bool_t* enabled);
    hal_result_t (*handle_hotplug_event)(hal_u32_t event_type, hal_u64_t event_data);
    
    // Memory compaction
    hal_result_t (*compact_memory)(void);
    hal_result_t (*defragment_memory)(void);
    hal_result_t (*is_compaction_needed)(hal_bool_t* needed);
    hal_result_t (*get_fragmentation_ratio)(hal_u32_t* ratio);
    
    // Memory leak detection
    hal_result_t (*enable_leak_detection)(void);
    hal_result_t (*disable_leak_detection)(void);
    hal_result_t (*is_leak_detection_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_leak_info)(hal_leak_info_t* info);
    hal_result_t (*dump_leaks)(void);
};

// Memory operation convenience functions
hal_result_t hal_memory_init(void);
hal_result_t hal_memory_shutdown(void);
hal_result_t hal_memory_reset(void);

void* hal_memory_alloc(hal_size_t size);
void* hal_memory_realloc(void* ptr, hal_size_t new_size);
void hal_memory_free(void* ptr);
void* hal_memory_calloc(hal_size_t count, hal_size_t size);
void* hal_memory_aligned_alloc(hal_size_t size, hal_size_t alignment);
void hal_memory_aligned_free(void* ptr);

hal_result_t hal_memory_map_physical(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
hal_result_t hal_memory_unmap(hal_virt_addr_t virt_addr, hal_size_t size);
hal_result_t hal_memory_remap(hal_virt_addr_t old_addr, hal_size_t old_size, hal_virt_addr_t* new_addr);

hal_result_t hal_memory_protect(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
hal_result_t hal_memory_unprotect(hal_virt_addr_t addr, hal_size_t size);
hal_result_t hal_memory_get_protection(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t* protection);

hal_result_t hal_memory_get_info(hal_memory_info_t* info);
hal_size_t hal_memory_get_total_memory(void);
hal_size_t hal_memory_get_available_memory(void);
hal_size_t hal_memory_get_used_memory(void);
hal_size_t hal_memory_get_free_memory(void);

hal_result_t hal_memory_get_stats(hal_memory_stats_t* stats);
hal_result_t hal_memory_reset_stats(void);
hal_result_t hal_memory_dump_stats(void);

// Memory testing and validation
hal_result_t hal_memory_test(hal_virt_addr_t addr, hal_size_t size);
hal_result_t hal_memory_test_pattern(hal_virt_addr_t addr, const hal_u8_t* pattern, hal_size_t size);
hal_result_t hal_memory_fill(hal_virt_addr_t addr, hal_u8_t value, hal_size_t size);
hal_result_t hal_memory_zero(hal_virt_addr_t addr, hal_size_t size);
hal_result_t hal_memory_copy(hal_virt_addr_t dest, const void* src, hal_size_t size);
hal_result_t hal_memory_move(hal_virt_addr_t dest, const void* src, hal_size_t size);
hal_result_t hal_memory_compare(const void* ptr1, const void* ptr2, hal_size_t size);

// Memory protection macros
#define HAL_MEMORY_PROTECT(addr, size, prot) hal_memory_protect((addr), (size), (prot))
#define HAL_MEMORY_UNPROTECT(addr, size) hal_memory_unprotect((addr), (size))
#define HAL_MEMORY_GET_PROTECTION(addr, size, prot) hal_memory_get_protection((addr), (size), (prot))

// Memory allocation macros
#define HAL_MEMORY_ALLOC(size) hal_memory_alloc((size))
#define HAL_MEMORY_REALLOC(ptr, new_size) hal_memory_realloc((ptr), (new_size))
#define HAL_MEMORY_FREE(ptr) hal_memory_free((ptr))
#define HAL_MEMORY_CALLOC(count, size) hal_memory_calloc((count), (size))
#define HAL_MEMORY_ALIGNED_ALLOC(size, align) hal_memory_aligned_alloc((size), (align))
#define HAL_MEMORY_ALIGNED_FREE(ptr) hal_memory_aligned_free((ptr))

// Memory mapping macros
#define HAL_MEMORY_MAP_PHYSICAL(phys, size, virt) hal_memory_map_physical((phys), (size), (virt))
#define HAL_MEMORY_UNMAP(virt, size) hal_memory_unmap((virt), (size))
#define HAL_MEMORY_REMAP(old, old_size, new, new_size) hal_memory_remap((old), (old_size), (new), (new))

// Memory info macros
#define HAL_MEMORY_GET_INFO(info) hal_memory_get_info(info)
#define HAL_MEMORY_GET_TOTAL() hal_memory_get_total_memory()
#define HAL_MEMORY_GET_AVAILABLE() hal_memory_get_available_memory()
#define HAL_MEMORY_GET_USED() hal_memory_get_used_memory()
#define HAL_MEMORY_GET_FREE() hal_memory_get_free_memory()

// Memory statistics macros
#define HAL_MEMORY_GET_STATS(stats) hal_memory_get_stats(stats)
#define HAL_MEMORY_RESET_STATS() hal_memory_reset_stats()
#define HAL_MEMORY_DUMP_STATS() hal_memory_dump_stats()

// Memory validation macros
#define HAL_MEMORY_VALIDATE_PARAM(param) HAL_VALIDATE_PARAM(param)
#define HAL_MEMORY_VALIDATE_STATE(condition) HAL_VALIDATE_STATE(condition)

// Memory barrier macros
#define HAL_MEMORY_BARRIER() hal_memory_barrier()
#define HAL_READ_BARRIER() hal_read_barrier()
#define HAL_WRITE_BARRIER() hal_write_barrier()
#define HAL_COMPILER_BARRIER() hal_compiler_barrier()
#define HAL_INSTRUCTION_BARRIER() hal_instruction_barrier()

// Memory alignment macros
#define HAL_MEMORY_ALIGN_UP(size, align) HAL_ALIGN_UP((size), (align))
#define HAL_MEMORY_ALIGN_DOWN(size, align) HAL_ALIGN_DOWN((size), (align))
#define HAL_MEMORY_IS_ALIGNED(addr, align) HAL_IS_ALIGNED((addr), (align))

// Memory size macros
#define HAL_MEMORY_KB (1024)
#define HAL_MEMORY_MB (1024 * HAL_MEMORY_KB)
#define HAL_MEMORY_GB (1024 * HAL_MEMORY_MB)
#define HAL_MEMORY_TB (1024 * HAL_MEMORY_GB)

// Memory type macros
#define HAL_MEMORY_IS_ALIGNED(addr, align) HAL_IS_ALIGNED((addr), (align))
#define HAL_MEMORY_IS_POWER_OF_TWO(x) HAL_IS_POWER_OF_TWO((x))
#define HAL_MEMORY_ALIGN_UP(size, align) HAL_ALIGN_UP((size), (align))
#define HAL_MEMORY_ALIGN_DOWN(size, align) HAL_ALIGN_DOWN((size), (align))
