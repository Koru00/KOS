#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Memory System Test Suite
// =============================================================================

// Test framework
typedef struct test_result {
    char name[64];
    bool passed;
    const char* error_msg;
} test_result_t;

static test_result_t g_test_results[32];
static int g_test_count = 0;

#define TEST_ASSERT(condition, msg) \
    do { \
        if (!(condition)) { \
            g_test_results[g_test_count].passed = false; \
            g_test_results[g_test_count].error_msg = msg; \
            return; \
        } \
    } while(0)

#define TEST_START(name_str) \
    do { \
        kos_strcpy(g_test_results[g_test_count].name, name_str); \
        g_test_results[g_test_count].passed = true; \
        g_test_results[g_test_count].error_msg = NULL; \
    } while(0)

#define TEST_END() \
    do { \
        g_test_count++; \
    } while(0)

// External memory manager
extern memory_manager_t g_memory_manager;

// Test 1: Basic Allocation
void test_basic_allocation(void) {
    TEST_START("Basic Allocation");
    
    // Test simple allocation
    void* ptr1 = kos_memory_alloc(&g_memory_manager, 64);
    TEST_ASSERT(ptr1 != NULL, "Failed to allocate 64 bytes");
    
    void* ptr2 = kos_memory_alloc(&g_memory_manager, 128);
    TEST_ASSERT(ptr2 != NULL, "Failed to allocate 128 bytes");
    
    // Test that pointers are different
    TEST_ASSERT(ptr1 != ptr2, "Allocated pointers should be different");
    
    // Test memory is writable
    char* mem1 = (char*)ptr1;
    mem1[0] = 'A';
    mem1[63] = 'Z';
    TEST_ASSERT(mem1[0] == 'A' && mem1[63] == 'Z', "Memory not writable");
    
    // Test memory is readable
    char* mem2 = (char*)ptr2;
    mem2[0] = 'X';
    mem2[127] = 'Y';
    TEST_ASSERT(mem2[0] == 'X' && mem2[127] == 'Y', "Memory not readable");
    
    // Clean up
    kos_memory_free(&g_memory_manager, ptr1);
    kos_memory_free(&g_memory_manager, ptr2);
    
    TEST_END();
}

// Test 2: Memory Leak Detection
void test_memory_leaks(void) {
    TEST_START("Memory Leak Detection");
    
    // Get initial memory state
    size_t initial_free = g_memory_manager.total_free;
    
    // Allocate and free memory
    void* ptr1 = kos_memory_alloc(&g_memory_manager, 32);
    TEST_ASSERT(ptr1 != NULL, "Failed to allocate 32 bytes");
    
    size_t after_alloc = g_memory_manager.total_free;
    TEST_ASSERT(after_alloc < initial_free, "Free memory should decrease after allocation");
    
    kos_memory_free(&g_memory_manager, ptr1);
    size_t after_free = g_memory_manager.total_free;
    TEST_ASSERT(after_free >= initial_free, "Free memory should be restored after free");
    
    TEST_END();
}

// Test 3: Edge Cases
void test_edge_cases(void) {
    TEST_START("Edge Cases");
    
    // Test allocation of size 0
    void* ptr0 = kos_memory_alloc(&g_memory_manager, 0);
    TEST_ASSERT(ptr0 == NULL, "Size 0 allocation should return NULL");
    
    // Test free of NULL pointer (should not crash)
    kos_memory_free(&g_memory_manager, NULL);
    // If we reach here, no crash occurred
    
    // Test very small allocation
    void* ptr1 = kos_memory_alloc(&g_memory_manager, 1);
    TEST_ASSERT(ptr1 != NULL, "Failed to allocate 1 byte");
    kos_memory_free(&g_memory_manager, ptr1);
    
    // Test large allocation
    void* ptr2 = kos_memory_alloc(&g_memory_manager, 4096);
    TEST_ASSERT(ptr2 != NULL, "Failed to allocate 4096 bytes");
    kos_memory_free(&g_memory_manager, ptr2);
    
    TEST_END();
}

// Test 4: Realloc Functionality
void test_realloc(void) {
    TEST_START("Realloc Functionality");
    
    // Initial allocation
    void* ptr1 = kos_memory_alloc(&g_memory_manager, 64);
    TEST_ASSERT(ptr1 != NULL, "Failed initial allocation");
    
    // Write test data
    char* mem = (char*)ptr1;
    for (int i = 0; i < 64; i++) {
        mem[i] = (char)(i % 256);
    }
    
    // Realloc to larger size
    void* ptr2 = kos_memory_realloc(&g_memory_manager, ptr1, 128);
    TEST_ASSERT(ptr2 != NULL, "Failed to reallocate to larger size");
    
    // Verify data is preserved
    char* new_mem = (char*)ptr2;
    bool data_preserved = true;
    for (int i = 0; i < 64; i++) {
        if (new_mem[i] != (char)(i % 256)) {
            data_preserved = false;
            break;
        }
    }
    TEST_ASSERT(data_preserved, "Data not preserved during realloc");
    
    // Realloc to smaller size
    void* ptr3 = kos_memory_realloc(&g_memory_manager, ptr2, 32);
    TEST_ASSERT(ptr3 != NULL, "Failed to reallocate to smaller size");
    
    // Verify first 32 bytes are preserved
    data_preserved = true;
    for (int i = 0; i < 32; i++) {
        if (new_mem[i] != (char)(i % 256)) {
            data_preserved = false;
            break;
        }
    }
    TEST_ASSERT(data_preserved, "Data not preserved during size reduction");
    
    // Realloc to NULL (should be equivalent to free)
    void* ptr4 = kos_memory_realloc(&g_memory_manager, ptr3, 0);
    TEST_ASSERT(ptr4 == NULL, "Realloc to size 0 should return NULL");
    
    TEST_END();
}

// Test 5: Memory Alignment
void test_memory_alignment(void) {
    TEST_START("Memory Alignment");
    
    // Test multiple allocations
    void* ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = kos_memory_alloc(&g_memory_manager, 17); // Odd size
        TEST_ASSERT(ptrs[i] != NULL, "Failed allocation");
        
        // Check 8-byte alignment
        uintptr_t addr = (uintptr_t)ptrs[i];
        TEST_ASSERT((addr % 8) == 0, "Memory not 8-byte aligned");
    }
    
    // Free all allocations
    for (int i = 0; i < 10; i++) {
        kos_memory_free(&g_memory_manager, ptrs[i]);
    }
    
    TEST_END();
}

// Test 6: Fragmentation Handling
void test_fragmentation(void) {
    TEST_START("Fragmentation Handling");
    
    void* ptrs[20];
    
    // Allocate many small blocks
    for (int i = 0; i < 20; i++) {
        ptrs[i] = kos_memory_alloc(&g_memory_manager, 64);
        TEST_ASSERT(ptrs[i] != NULL, "Failed small allocation");
    }
    
    // Free every other block to create fragmentation
    for (int i = 0; i < 20; i += 2) {
        kos_memory_free(&g_memory_manager, ptrs[i]);
    }
    
    // Try to allocate a larger block that should fit in coalesced space
    void* large_ptr = kos_memory_alloc(&g_memory_manager, 512);
    TEST_ASSERT(large_ptr != NULL, "Failed to allocate in fragmented space");
    
    // Free remaining blocks
    for (int i = 1; i < 20; i += 2) {
        kos_memory_free(&g_memory_manager, ptrs[i]);
    }
    
    kos_memory_free(&g_memory_manager, large_ptr);
    
    TEST_END();
}

// Test runner
void run_memory_tests(void) {
    log_info("Starting Memory System Tests...");
    
    // Initialize test counter
    g_test_count = 0;
    
    // Run all tests
    test_basic_allocation();
    test_memory_leaks();
    test_edge_cases();
    test_realloc();
    test_memory_alignment();
    test_fragmentation();
    
    // Report results
    int passed = 0;
    int failed = 0;
    
    log_info("Memory Test Results:");
    for (int i = 0; i < g_test_count; i++) {
        if (g_test_results[i].passed) {
            log_info("  ✓ %s", g_test_results[i].name);
            passed++;
        } else {
            log_error("  ✗ %s: %s", g_test_results[i].name, g_test_results[i].error_msg);
            failed++;
        }
    }
    
    log_info("Memory Tests Summary: %d passed, %d failed", passed, failed);
    
    if (failed == 0) {
        log_info("All memory tests PASSED!");
    } else {
        log_error("Some memory tests FAILED!");
    }
}

// Entry point for memory testing
void test_memory_system(void) {
    // Initialize memory manager if not already done
    kos_result_t result = kos_memory_init(&g_memory_manager);
    if (result != KOS_SUCCESS) {
        log_error("Failed to initialize memory manager for testing");
        return;
    }
    
    // Run the test suite
    run_memory_tests();
}
