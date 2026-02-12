#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// KOS - Test Runner Interface
// =============================================================================

// Test runner API
void kos_run_tests(void);
void kos_run_quick_test(void);
void kos_run_memory_tests(void);
void kos_run_logging_tests(void);
void kos_run_integration_tests(void);

// Test suite configuration
void enable_test_suite(const char* name, bool enabled);
void list_test_suites(void);

// Test execution modes
void run_all_tests(void);
void run_test_suite_by_name(const char* name);
void run_quick_validation(void);
void run_comprehensive_validation(void);

// Test runner configuration
void configure_test_runner(bool verbose);

// Test macros for convenience
#define RUN_ALL_TESTS() kos_run_tests()
#define RUN_QUICK_TEST() kos_run_quick_test()
#define RUN_MEMORY_TESTS() kos_run_memory_tests()
#define RUN_LOGGING_TESTS() kos_run_logging_tests()
#define RUN_INTEGRATION_TESTS() kos_run_integration_tests()

// Test result checking macros
#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            log_error("EXPECT_TRUE failed: %s", #condition); \
            return false; \
        } \
    } while(0)

#define EXPECT_FALSE(condition) \
    do { \
        if (condition) { \
            log_error("EXPECT_FALSE failed: %s", #condition); \
            return false; \
        } \
    } while(0)

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            log_error("EXPECT_EQ failed: expected %d, got %d", (int)(expected), (int)(actual)); \
            return false; \
        } \
    } while(0)

#define EXPECT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            log_error("EXPECT_NE failed: values should not be equal (%d)", (int)(actual)); \
            return false; \
        } \
    } while(0)

#define EXPECT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            log_error("EXPECT_NULL failed: pointer is not NULL (%p)", (void*)(ptr)); \
            return false; \
        } \
    } while(0)

#define EXPECT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            log_error("EXPECT_NOT_NULL failed: pointer is NULL"); \
            return false; \
        } \
    } while(0)
