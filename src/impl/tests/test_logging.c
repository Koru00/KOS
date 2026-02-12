#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kos/utils/string.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Logging System Test Suite
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

// Test 1: Basic Logging
void test_basic_logging(void) {
    TEST_START("Basic Logging");
    
    // Test basic log functions (they should not crash)
    log_init();
    
    // Test different log levels
    log_debug("Debug message");
    log_info("Info message");
    log_warn("Warning message");
    log_error("Error message");
    log_critical("Critical message");
    
    // If we reach here, no crash occurred
    TEST_END();
}

// Test 2: va_args Functionality
void test_va_args(void) {
    TEST_START("va_args Functionality");
    
    // Test string formatting
    log_debug("Test string: %s", "Hello World");
    log_info("Test integer: %d", 42);
    log_warn("Test hex: %x", 0xFF);
    log_error("Test unsigned: %u", 12345);
    log_critical("Test character: %c", 'A');
    
    // Test multiple arguments
    log_info("Multiple args: %s %d %x %c", "test", 123, 0xAB, 'Z');
    
    // Test percent sign
    log_debug("Percent sign: %%");
    
    // If we reach here, va_args worked
    TEST_END();
}

// Test 3: Log Levels
void test_log_levels(void) {
    TEST_START("Log Levels");
    
    // Test all log levels with formatting
    log_debug("DEBUG level test: %d", 1);
    log_info("INFO level test: %d", 2);
    log_warn("WARN level test: %d", 3);
    log_error("ERROR level test: %d", 4);
    log_critical("CRITICAL level test: %d", 5);
    
    // Test log_printf with different levels
    log_printf(LOG_LEVEL_DEBUG, "DEBUG via printf: %s", "test");
    log_printf(LOG_LEVEL_INFO, "INFO via printf: %d", 42);
    log_printf(LOG_LEVEL_WARN, "WARN via printf: %x", 0xFF);
    log_printf(LOG_LEVEL_ERROR, "ERROR via printf: %u", 123);
    log_printf(LOG_LEVEL_CRITICAL, "CRITICAL via printf: %c", '!');
    
    TEST_END();
}

// Test 4: Serial Output
void test_serial_output(void) {
    TEST_START("Serial Output");
    
    // Initialize serial port
    init_serial();
    
    // Test direct serial output
    serial_print("Direct serial test\n");
    
    // Test serial with formatting
    serial_print("Formatted serial: %s %d %x\n", "test", 123, 0xAB);
    
    // Test edge cases
    serial_print("Empty string: ");
    serial_print("");
    serial_print("\n");
    
    serial_print("Special chars: %s\n", "Hello\tWorld\nTest");
    
    TEST_END();
}

// Test 5: Format String Edge Cases
void test_format_edge_cases(void) {
    TEST_START("Format String Edge Cases");
    
    // Test empty format string
    log_debug("");
    
    // Test format with only percent signs
    log_debug("%% %% %%");
    
    // Test invalid format specifiers (should not crash)
    log_debug("Invalid: %q %r %t");
    
    // Test very long strings
    char long_string[256];
    for (int i = 0; i < 255; i++) {
        long_string[i] = 'A';
    }
    long_string[255] = '\0';
    log_debug("Long string: %s", long_string);
    
    // Test negative numbers
    log_debug("Negative: %d", -123);
    log_debug("Negative hex: %x", -1);
    
    // Test zero values
    log_debug("Zero int: %d", 0);
    log_debug("Zero hex: %x", 0);
    log_debug("Zero unsigned: %u", 0);
    
    // Test large numbers
    log_debug("Large int: %d", 2147483647);
    log_debug("Large hex: %x", 0xFFFFFFFF);
    log_debug("Large unsigned: %u", 4294967295U);
    
    TEST_END();
}

// Test 6: Legacy Compatibility
void test_legacy_compatibility(void) {
    TEST_START("Legacy Compatibility");
    
    // Test legacy log_message function
    log_message("test_function", "Legacy test message", LOG_INFO);
    log_message("test_function", "Legacy warning", LOG_WARNING);
    log_message("test_function", "Legacy error", LOG_ERROR);
    
    // Test legacy serial_log
    serial_log();
    
    // Test convenience macros
    LOG_DEBUG("Debug macro: %d", 1);
    LOG_INFO("Info macro: %d", 2);
    LOG_WARN("Warn macro: %d", 3);
    LOG_ERROR("Error macro: %d", 4);
    LOG_CRITICAL("Critical macro: %d", 5);
    
    TEST_END();
}

// Test 7: Performance Logging
void test_performance_logging(void) {
    TEST_START("Performance Logging");
    
    // Test logging performance (should not be too slow)
    for (int i = 0; i < 100; i++) {
        log_debug("Performance test iteration %d", i);
    }
    
    // Test different log levels in loop
    for (int i = 0; i < 10; i++) {
        log_info("Loop test %d: %s %d", i, "value", i * 10);
        log_warn("Warning loop %d", i);
        log_error("Error loop %d: %x", i * 0x10);
    }
    
    TEST_END();
}

// Test runner
void run_logging_tests(void) {
    log_info("Starting Logging System Tests...");
    
    // Initialize test counter
    g_test_count = 0;
    
    // Run all tests
    test_basic_logging();
    test_va_args();
    test_log_levels();
    test_serial_output();
    test_format_edge_cases();
    test_legacy_compatibility();
    test_performance_logging();
    
    // Report results
    int passed = 0;
    int failed = 0;
    
    log_info("Logging Test Results:");
    for (int i = 0; i < g_test_count; i++) {
        if (g_test_results[i].passed) {
            log_info("  ✓ %s", g_test_results[i].name);
            passed++;
        } else {
            log_error("  ✗ %s: %s", g_test_results[i].name, g_test_results[i].error_msg);
            failed++;
        }
    }
    
    log_info("Logging Tests Summary: %d passed, %d failed", passed, failed);
    
    if (failed == 0) {
        log_info("All logging tests PASSED!");
    } else {
        log_error("Some logging tests FAILED!");
    }
}

// Entry point for logging testing
void test_logging_system(void) {
    // Initialize logging system
    log_init();
    
    // Run the test suite
    run_logging_tests();
}
