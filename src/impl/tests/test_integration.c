#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "kos/kernel/kernel.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Integration Test Suite
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

// External components
extern memory_manager_t g_memory_manager;

// Test 1: Kernel Startup Sequence
void test_kernel_startup(void) {
    TEST_START("Kernel Startup Sequence");
    
    // Test kernel initialization
    kos_result_t result = kos_kernel_init(NULL); // Use default config
    TEST_ASSERT(result == KOS_SUCCESS, "Kernel initialization failed");
    
    // Test kernel version
    const char* version = kos_kernel_get_version();
    TEST_ASSERT(version != NULL, "Kernel version should not be NULL");
    TEST_ASSERT(kos_strlen(version) > 0, "Kernel version should not be empty");
    
    // Log kernel version
    log_info("Kernel version: %s", version);
    
    TEST_END();
}

// Test 2: Memory System Integration
void test_memory_integration(void) {
    TEST_START("Memory System Integration");
    
    // Initialize memory manager
    kos_result_t result = kos_memory_init(&g_memory_manager);
    TEST_ASSERT(result == KOS_SUCCESS, "Memory manager initialization failed");
    
    // Test memory allocation during kernel operation
    void* test_buffer = kos_memory_alloc(&g_memory_manager, 1024);
    TEST_ASSERT(test_buffer != NULL, "Failed to allocate test buffer");
    
    // Test memory operations with logging
    log_info("Allocated test buffer at address: %x", (uintptr_t)test_buffer);
    
    // Write test pattern
    char* buffer = (char*)test_buffer;
    for (int i = 0; i < 1024; i++) {
        buffer[i] = (char)(i % 256);
    }
    
    // Verify pattern
    bool pattern_valid = true;
    for (int i = 0; i < 1024; i++) {
        if (buffer[i] != (char)(i % 256)) {
            pattern_valid = false;
            break;
        }
    }
    TEST_ASSERT(pattern_valid, "Memory pattern verification failed");
    
    log_info("Memory pattern verification: %s", pattern_valid ? "PASSED" : "FAILED");
    
    // Free memory
    kos_memory_free(&g_memory_manager, test_buffer);
    
    // Test memory statistics
    log_info("Memory stats - Total free: %u, Total allocated: %u", 
             g_memory_manager.total_free, g_memory_manager.total_allocated);
    
    TEST_END();
}

// Test 3: String Utilities Integration
void test_string_integration(void) {
    TEST_START("String Utilities Integration");
    
    // Test string operations
    char test_string[64];
    kos_strcpy(test_string, "Hello KOS");
    TEST_ASSERT(kos_strcmp(test_string, "Hello KOS") == 0, "String copy/compare failed");
    
    // Test string formatting
    char number_str[32];
    kos_itoa(12345, number_str, 10);
    TEST_ASSERT(kos_strcmp(number_str, "12345") == 0, "Integer to string conversion failed");
    
    kos_uitoa(0xABCD, number_str, 16);
    TEST_ASSERT(kos_strcmp(number_str, "ABCD") == 0, "Hex to string conversion failed");
    
    // Test memory operations with strings
    char source[] = "Test memory copy";
    char dest[64];
    kos_memcpy(dest, source, kos_strlen(source) + 1);
    TEST_ASSERT(kos_strcmp(source, dest) == 0, "Memory copy with strings failed");
    
    // Test memory set
    char memset_test[32];
    kos_memset(memset_test, 'A', 31);
    memset_test[31] = '\0';
    
    bool memset_correct = true;
    for (int i = 0; i < 31; i++) {
        if (memset_test[i] != 'A') {
            memset_correct = false;
            break;
        }
    }
    TEST_ASSERT(memset_correct, "Memory set operation failed");
    
    log_info("String utilities test: %s", memset_correct ? "PASSED" : "FAILED");
    
    TEST_END();
}

// Test 4: Logging During Kernel Operations
void test_logging_integration(void) {
    TEST_START("Logging During Kernel Operations");
    
    // Initialize logging
    log_init();
    
    // Test logging during memory operations
    void* ptr = kos_memory_alloc(&g_memory_manager, 256);
    log_debug("Allocated memory during logging test: %x", (uintptr_t)ptr);
    
    // Test logging with formatted strings
    log_info("Logging integration test: %s %d %x", "test", 42, 0xFF);
    
    // Test logging with string operations
    char test_msg[] = "Integration test message";
    log_info("String length test: %s (length: %u)", test_msg, kos_strlen(test_msg));
    
    // Test logging during memory operations
    kos_memory_free(&g_memory_manager, ptr);
    log_debug("Freed memory during logging test");
    
    // Test all log levels
    log_debug("Debug level integration test");
    log_info("Info level integration test");
    log_warn("Warning level integration test");
    log_error("Error level integration test");
    log_critical("Critical level integration test");
    
    TEST_END();
}

// Test 5: Component Interaction
void test_component_interaction(void) {
    TEST_START("Component Interaction");
    
    // Test memory + logging interaction
    void* buffers[10];
    for (int i = 0; i < 10; i++) {
        buffers[i] = kos_memory_alloc(&g_memory_manager, 64);
        TEST_ASSERT(buffers[i] != NULL, "Failed to allocate buffer in interaction test");
        
        // Log allocation
        log_debug("Allocated buffer %d at address: %x", i, (uintptr_t)buffers[i]);
        
        // Write test data using string utilities
        char* buffer = (char*)buffers[i];
        kos_strcpy(buffer, "Test data");
        kos_itoa(i, buffer + kos_strlen(buffer), 10);
    }
    
    // Verify all buffers
    bool all_valid = true;
    for (int i = 0; i < 10; i++) {
        char expected[32];
        kos_strcpy(expected, "Test data");
        kos_itoa(i, expected + kos_strlen(expected), 10);
        
        if (kos_strcmp((char*)buffers[i], expected) != 0) {
            all_valid = false;
            log_error("Buffer %d validation failed", i);
            break;
        }
    }
    TEST_ASSERT(all_valid, "Component interaction validation failed");
    
    // Free all buffers with logging
    for (int i = 0; i < 10; i++) {
        log_debug("Freeing buffer %d", i);
        kos_memory_free(&g_memory_manager, buffers[i]);
    }
    
    log_info("Component interaction test: %s", all_valid ? "PASSED" : "FAILED");
    
    TEST_END();
}

// Test 6: Error Handling Integration
void test_error_handling(void) {
    TEST_START("Error Handling Integration");
    
    // Test memory allocation failure handling
    void* huge_ptr = kos_memory_alloc(&g_memory_manager, 0xFFFFFFFF);
    TEST_ASSERT(huge_ptr == NULL, "Huge allocation should fail");
    log_debug("Huge allocation properly failed");
    
    // Test invalid parameter handling
    kos_result_t result = kos_memory_init(NULL);
    TEST_ASSERT(result == KOS_ERROR_INVALID_PARAM, "NULL parameter should return error");
    log_debug("Invalid parameter properly handled");
    
    // Test logging with invalid parameters (should not crash)
    log_debug(NULL); // Should not crash
    log_info("%s", NULL); // Should not crash
    
    // Test kernel assert
    kos_kernel_assert(true, "This should not trigger panic");
    log_debug("Kernel assert with true condition passed");
    
    // Test string utilities with edge cases
    char* null_str = NULL;
    size_t len = kos_strlen(null_str);
    TEST_ASSERT(len == 0, "strlen of NULL should return 0");
    
    log_info("Error handling integration test completed");
    
    TEST_END();
}

// Test 7: Performance Integration
void test_performance_integration(void) {
    TEST_START("Performance Integration");
    
    // Test memory allocation performance
    uint32_t start_time = 0; // Would use timer in real implementation
    void* ptrs[100];
    
    for (int i = 0; i < 100; i++) {
        ptrs[i] = kos_memory_alloc(&g_memory_manager, 64);
        TEST_ASSERT(ptrs[i] != NULL, "Performance allocation failed");
    }
    
    // Test string operations performance
    char test_strings[100][32];
    for (int i = 0; i < 100; i++) {
        kos_itoa(i, test_strings[i], 10);
        kos_strcat(test_strings[i], "_test");
    }
    
    // Test logging performance
    for (int i = 0; i < 50; i++) {
        log_debug("Performance test iteration %d: %s", i, test_strings[i % 100]);
    }
    
    // Cleanup
    for (int i = 0; i < 100; i++) {
        kos_memory_free(&g_memory_manager, ptrs[i]);
    }
    
    log_info("Performance integration test completed");
    
    TEST_END();
}

// Test runner
void run_integration_tests(void) {
    log_info("Starting Integration Tests...");
    
    // Initialize test counter
    g_test_count = 0;
    
    // Run all tests
    test_kernel_startup();
    test_memory_integration();
    test_string_integration();
    test_logging_integration();
    test_component_interaction();
    test_error_handling();
    test_performance_integration();
    
    // Report results
    int passed = 0;
    int failed = 0;
    
    log_info("Integration Test Results:");
    for (int i = 0; i < g_test_count; i++) {
        if (g_test_results[i].passed) {
            log_info("  ✓ %s", g_test_results[i].name);
            passed++;
        } else {
            log_error("  ✗ %s: %s", g_test_results[i].name, g_test_results[i].error_msg);
            failed++;
        }
    }
    
    log_info("Integration Tests Summary: %d passed, %d failed", passed, failed);
    
    if (failed == 0) {
        log_info("All integration tests PASSED!");
    } else {
        log_error("Some integration tests FAILED!");
    }
}

// Entry point for integration testing
void test_integration_system(void) {
    // Initialize all systems
    log_init();
    kos_memory_init(&g_memory_manager);
    
    // Run the test suite
    run_integration_tests();
}
