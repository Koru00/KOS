#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../utils/string.h"
#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_cpu.h"
#include "hal/hal_interrupt_simple.h"
#include "hal/hal_memory_simple.h"
#include "hal/hal_timer_simple.h"
#include "hal/hal_platform_simple.h"

// =============================================================================
// KOS - Memory Management Implementation (Standalone)
// =============================================================================

// Basic types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;
typedef size_t    usize;
typedef _Bool    b8;

typedef enum {
    KOS_SUCCESS = 0,
    KOS_ERROR_INVALID_PARAM = -1,
    KOS_ERROR_OUT_OF_MEMORY = -2,
    KOS_ERROR_INVALID_STATE = -8
} kos_result_t;

// Configuration
#define KOS_HEAP_START_ADDR    0x200000  // 2MB
#define KOS_HEAP_SIZE          0x200000  // 2MB
#define KOS_MEMORY_DEFAULT_ALIGNMENT 8

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

// Global memory manager instance
memory_manager_t g_memory_manager;

// Forward declarations
void kos_memory_free(memory_manager_t* manager, void* ptr);

kos_result_t kos_memory_init(memory_manager_t* manager) {
    if (!manager) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Initialize HAL memory first
    hal_result_t hal_result = hal_memory_init();
    if (hal_result != HAL_SUCCESS) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    // Initialize manager structure
    kos_memset(manager, 0, sizeof(memory_manager_t));
    
    // Set up heap using HAL memory allocation
    void* heap_start = hal_memory_alloc(KOS_HEAP_SIZE);
    if (!heap_start) {
        return KOS_ERROR_OUT_OF_MEMORY;
    }
    
    manager->heap_start = heap_start;
    manager->heap_size = KOS_HEAP_SIZE;
    
    // Initialize free list
    memory_block_t* first_block = (memory_block_t*)manager->heap_start;
    first_block->size = KOS_HEAP_SIZE - sizeof(memory_block_t);
    first_block->next = NULL;
    first_block->is_free = true;
    
    manager->free_list = first_block;
    manager->total_allocated = 0;
    manager->total_free = first_block->size;
    
    return KOS_SUCCESS;
}

static b8 is_valid_pointer(memory_manager_t* manager, void* ptr) {
    if (!ptr || !manager) {
        return false;
    }
    
    u8* byte_ptr = (u8*)ptr;
    u8* heap_start = (u8*)manager->heap_start;
    
    return (byte_ptr >= heap_start && byte_ptr < heap_start + manager->heap_size);
}

static b8 is_valid_block(memory_manager_t* manager, memory_block_t* block) {
    if (!is_valid_pointer(manager, block)) {
        return false;
    }
    
    // Check if block size is reasonable
    if (block->size > manager->heap_size || block->size < sizeof(memory_block_t)) {
        return false;
    }
    
    return true;
}

void* kos_memory_alloc(memory_manager_t* manager, usize size) {
    if (!manager || size == 0) {
        return NULL;
    }
    
    // Align size to 8-byte boundary
    size = (size + KOS_MEMORY_DEFAULT_ALIGNMENT - 1) & ~(KOS_MEMORY_DEFAULT_ALIGNMENT - 1);
    
    memory_block_t* current = manager->free_list;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            // Check if we can split the block
            if (current->size >= size + sizeof(memory_block_t) + 8) {
                memory_block_t* new_block = (memory_block_t*)((u8*)current + sizeof(memory_block_t) + size);
                new_block->size = current->size - size - sizeof(memory_block_t);
                new_block->next = current->next;
                new_block->is_free = true;
                
                current->size = size;
                current->next = new_block;
                
                manager->total_free -= sizeof(memory_block_t);
            }
            
            current->is_free = false;
            manager->total_allocated += size;
            manager->total_free -= size;
            
            return (void*)((u8*)current + sizeof(memory_block_t));
        }
        
        current = current->next;
    }
    
    return NULL;
}

void* kos_memory_realloc(memory_manager_t* manager, void* ptr, usize new_size) {
    if (!manager) {
        return NULL;
    }
    
    if (!ptr) {
        return kos_memory_alloc(manager, new_size);
    }
    
    if (new_size == 0) {
        kos_memory_free(manager, ptr);
        return NULL;
    }
    
    // Validate input pointer
    if (!is_valid_pointer(manager, ptr)) {
        return NULL;
    }
    
    memory_block_t* old_block = (memory_block_t*)((u8*)ptr - sizeof(memory_block_t));
    
    // Validate block header
    if (!is_valid_block(manager, old_block)) {
        return NULL;
    }
    
    // Align new size
    new_size = (new_size + KOS_MEMORY_DEFAULT_ALIGNMENT - 1) & ~(KOS_MEMORY_DEFAULT_ALIGNMENT - 1);
    
    // If new size fits in current block, return same pointer
    if (old_block->size >= new_size) {
        return ptr;
    }
    
    // Allocate new block
    void* new_ptr = kos_memory_alloc(manager, new_size);
    if (!new_ptr) {
        return NULL;
    }
    
    // Copy old contents to new block
    usize copy_size = old_block->size < new_size ? old_block->size : new_size;
    kos_memcpy(new_ptr, ptr, copy_size);
    
    // Free old block
    kos_memory_free(manager, ptr);
    
    return new_ptr;
}

void kos_memory_free(memory_manager_t* manager, void* ptr) {
    if (!manager || !ptr) {
        return;
    }
    
    // Validate pointer is within heap bounds
    if (!is_valid_pointer(manager, ptr)) {
        return;
    }
    
    memory_block_t* block = (memory_block_t*)((u8*)ptr - sizeof(memory_block_t));
    
    // Validate block header
    if (!is_valid_block(manager, block)) {
        return;
    }
    
    block->is_free = true;
    manager->total_allocated -= block->size;
    manager->total_free += block->size;
    
    // Coalesce with next block if it's free
    if (block->next && block->next->is_free) {
        block->size += sizeof(memory_block_t) + block->next->size;
        block->next = block->next->next;
        manager->total_free += sizeof(memory_block_t);
    }
}
