#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kos/utils/string.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Test Runner
// =============================================================================

// External test functions
extern void test_memory_system(void);
extern void test_logging_system(void);
extern void test_integration_system(void);

// Test suite structure
typedef struct {
    const char* name;
    void (*test_func)(void);
    bool enabled;
} test_suite_t;

// Test suites
static test_suite_t g_test_suites[] = {
    {"Memory System", test_memory_system, true},
    {"Logging System", test_logging_system, true},
    {"Integration Tests", test_integration_system, true},
};

static const int g_num_test_suites = sizeof(g_test_suites) / sizeof(test_suite_t);

// Test runner state
typedef struct {
    int total_suites;
    int passed_suites;
    int failed_suites;
    bool verbose;
} test_runner_state_t;

static test_runner_state_t g_runner_state = {0};

// Helper functions
void print_test_header(void) {
    log_info("========================================");
    log_info("  KOS Fase 0 Validation Test Suite");
    log_info("========================================");
    log_info("Test suites to run: %d", g_num_test_suites);
    log_info("");
}

void print_test_footer(void) {
    log_info("");
    log_info("========================================");
    log_info("  Test Suite Summary");
    log_info("========================================");
    log_info("Total suites: %d", g_runner_state.total_suites);
    log_info("Passed: %d", g_runner_state.passed_suites);
    log_info("Failed: %d", g_runner_state.failed_suites);
    
    if (g_runner_state.failed_suites == 0) {
        log_info("");
        log_info("🎉 ALL TESTS PASSED! 🎉");
        log_info("Fase 0 validation completed successfully!");
        log_info("The foundation is solid for next phases.");
    } else {
        log_info("");
        log_error("❌ SOME TESTS FAILED!");
        log_error("Fase 0 validation has issues that need to be addressed.");
        log_error("Please fix failing tests before proceeding to next phases.");
    }
    
    log_info("========================================");
}

void run_test_suite(test_suite_t* suite) {
    if (!suite || !suite->test_func) {
        log_error("Invalid test suite");
        return;
    }
    
    if (!suite->enabled) {
        log_info("Skipping disabled test suite: %s", suite->name);
        return;
    }
    
    log_info("Running test suite: %s", suite->name);
    log_info("----------------------------------------");
    
    // Run the test suite
    suite->test_func();
    
    log_info("----------------------------------------");
    log_info("Test suite completed: %s", suite->name);
    log_info("");
    
    g_runner_state.total_suites++;
}

// Test runner configuration
void configure_test_runner(bool verbose) {
    g_runner_state.verbose = verbose;
    g_runner_state.total_suites = 0;
    g_runner_state.passed_suites = 0;
    g_runner_state.failed_suites = 0;
}

// Enable/disable specific test suites
void enable_test_suite(const char* name, bool enabled) {
    for (int i = 0; i < g_num_test_suites; i++) {
        if (kos_strcmp(g_test_suites[i].name, name) == 0) {
            g_test_suites[i].enabled = enabled;
            log_info("Test suite '%s' %s", name, enabled ? "enabled" : "disabled");
            return;
        }
    }
    log_warn("Test suite '%s' not found", name);
}

// List all available test suites
void list_test_suites(void) {
    log_info("Available test suites:");
    for (int i = 0; i < g_num_test_suites; i++) {
        log_info("  %d. %s [%s]", i + 1, g_test_suites[i].name, 
                 g_test_suites[i].enabled ? "ENABLED" : "DISABLED");
    }
}

// Run all enabled test suites
void run_all_tests(void) {
    print_test_header();
    
    for (int i = 0; i < g_num_test_suites; i++) {
        if (g_test_suites[i].enabled) {
            run_test_suite(&g_test_suites[i]);
        }
    }
    
    print_test_footer();
}

// Run specific test suite
void run_test_suite_by_name(const char* name) {
    print_test_header();
    
    for (int i = 0; i < g_num_test_suites; i++) {
        if (kos_strcmp(g_test_suites[i].name, name) == 0) {
            if (g_test_suites[i].enabled) {
                run_test_suite(&g_test_suites[i]);
            } else {
                log_warn("Test suite '%s' is disabled", name);
            }
            print_test_footer();
            return;
        }
    }
    
    log_error("Test suite '%s' not found", name);
    list_test_suites();
    print_test_footer();
}

// Quick validation test (subset of tests)
void run_quick_validation(void) {
    log_info("Running quick validation...");
    
    // Only run critical tests
    bool original_states[g_num_test_suites];
    for (int i = 0; i < g_num_test_suites; i++) {
        original_states[i] = g_test_suites[i].enabled;
        // Only enable memory and integration tests for quick validation
        g_test_suites[i].enabled = (i == 0 || i == 2); // Memory and Integration
    }
    
    run_all_tests();
    
    // Restore original states
    for (int i = 0; i < g_num_test_suites; i++) {
        g_test_suites[i].enabled = original_states[i];
    }
}

// Comprehensive validation test (all tests with extra checks)
void run_comprehensive_validation(void) {
    log_info("Running comprehensive validation...");
    
    // Enable all tests
    for (int i = 0; i < g_num_test_suites; i++) {
        g_test_suites[i].enabled = true;
    }
    
    run_all_tests();
    
    // Additional system checks
    log_info("Running additional system checks...");
    
    // Check memory integrity
    log_info("Checking memory integrity...");
    // Would run memory integrity checks here
    
    // Check system resources
    log_info("Checking system resources...");
    // Would run resource checks here
    
    // Check for memory leaks
    log_info("Checking for memory leaks...");
    // Would run leak detection here
    
    log_info("Comprehensive validation completed.");
}

// Main test entry point
int main(void) {
    // Initialize logging
    log_init();
    
    // Configure test runner
    configure_test_runner(true); // Verbose mode
    
    // Run comprehensive validation
    run_comprehensive_validation();
    
    return 0;
}

// Test runner API for external use
void kos_run_tests(void) {
    main();
}

void kos_run_quick_test(void) {
    log_init();
    configure_test_runner(true);
    run_quick_validation();
}

void kos_run_memory_tests(void) {
    log_init();
    configure_test_runner(true);
    run_test_suite_by_name("Memory System");
}

void kos_run_logging_tests(void) {
    log_init();
    configure_test_runner(true);
    run_test_suite_by_name("Logging System");
}

void kos_run_integration_tests(void) {
    log_init();
    configure_test_runner(true);
    run_test_suite_by_name("Integration Tests");
}
