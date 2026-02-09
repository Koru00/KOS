#pragma once

#include "../types.h"
#include "../config.h"
#include <stddef.h>

// =============================================================================
// KOS - Memory Management Interface
// =============================================================================

// Memory block header
typedef struct memory_block {
    usize size;
    struct memory_block* next;
    b8   is_free;
    u8   padding[7];  // Alignment padding
} memory_block_t;

// Memory manager
typedef struct memory_manager {
    memory_block_t* free_list;
    void* heap_start;
    usize heap_size;
    usize total_allocated;
    usize total_free;
} memory_manager_t;

// Memory management functions
kos_result_t kos_memory_init(memory_manager_t* manager);
void* kos_memory_alloc(memory_manager_t* manager, usize size);
void* kos_memory_realloc(memory_manager_t* manager, void* ptr, usize new_size);
void kos_memory_free(memory_manager_t* manager, void* ptr);

// Utility functions
usize kos_memory_get_allocated(memory_manager_t* manager);
usize kos_memory_get_free(memory_manager_t* manager);
b8 kos_memory_is_valid_pointer(memory_manager_t* manager, void* ptr);

// Memory alignment
#define KOS_MEMORY_ALIGN(size, alignment) (((size) + (alignment) - 1) & ~((alignment) - 1))
#define KOS_MEMORY_DEFAULT_ALIGNMENT 8

// Convenience macros (legacy compatibility)
#define kos_malloc(size) kos_memory_alloc(&g_memory_manager, (size))
#define kos_realloc(ptr, size) kos_memory_realloc(&g_memory_manager, (ptr), (size))
#define kos_free(ptr) kos_memory_free(&g_memory_manager, (ptr))

// Global memory manager instance
extern memory_manager_t g_memory_manager;
