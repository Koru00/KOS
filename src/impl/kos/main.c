#include <stddef.h>
#include "performance/perf_monitor.h"
#include "performance/perf_shell.h"
#include "debug/debug.h"
#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_cpu.h"
#include "hal/hal_interrupt_simple.h"
#include "hal/hal_memory_simple.h"
#include "hal/hal_timer_simple.h"
#include "hal/hal_platform_simple.h"

// =============================================================================
// KOS - Main Entry Point (Fixed)
// =============================================================================

// Basic types
typedef enum {
    KOS_SUCCESS = 0,
    KOS_ERROR_INVALID_PARAM = -1
} kos_result_t;

// External test functions
extern void test_memory_system(void);
extern void test_logging_system(void);
extern void test_integration_system(void);

// Run validation tests
void run_validation_tests(void) {
    log_info("========================================");
    log_info("  KOS Fase 0 Validation Tests");
    log_info("========================================");
    
    // Initialize systems
    log_init();
    
    // Run test suites
    log_info("Running Memory System Tests...");
    test_memory_system();
    
    log_info("Running Logging System Tests...");
    test_logging_system();
    
    log_info("Running Integration Tests...");
    test_integration_system();
    
    log_info("========================================");
    log_info("  Validation Tests Completed");
    log_info("========================================");
}

// Legacy entry point - delegates to new kernel system
void kernel_main(void) {
    // Initialize HAL first - this is the most important step
    log_info("Initializing HAL...");
    hal_result_t hal_result = hal_init();
    if (hal_result != HAL_SUCCESS) {
        log_info("HAL initialization failed with code %d", hal_result);
        while (1) {
            asm volatile("hlt");
        }
    }
    log_info("HAL initialized successfully");
    
    // Get HAL interface and log some info
    hal_interface_t* hal_interface = hal_get_interface();
    if (hal_interface) {
        log_info("HAL Architecture: %s (%d-bit)", 
                hal_interface->arch_name, hal_interface->arch_bits);
        
        // Get CPU info
        hal_cpu_info_t cpu_info;
        if (hal_cpu_get_info(&cpu_info) == HAL_SUCCESS) {
            log_info("CPU: %s %s", cpu_info.vendor, cpu_info.model);
            log_info("CPU Cores: %u, Threads: %u", cpu_info.cores, cpu_info.threads);
        }
        
        // Get platform info
        hal_platform_info_t platform_info;
        if (hal_platform_get_info(&platform_info) == HAL_SUCCESS) {
            log_info("Platform: %s", platform_info.platform_name);
            log_info("Manufacturer: %s", platform_info.manufacturer);
        }
    }
    
    // Initialize performance monitoring first
    perf_init();
    perf_shell_init();
    
    // Log system start
    extern void log_info(const char* format, ...);
    log_info("KOS Kernel Starting...");
    log_info("Performance Monitor Active");
    
    // Run validation tests first
    log_info("Starting Fase 0 Validation...");
    run_validation_tests();
    
    // Call the standalone kernel initialization
    extern void kos_kernel_start(void);
    
    // Initialize kernel with default config
    extern kos_result_t kos_kernel_init(const void* config);
    kos_result_t result = kos_kernel_init(NULL);
    
    if (result != 0) {
        // If initialization fails, halt
        log_info("Kernel initialization failed with code %d", result);
        while (1) {
            asm volatile("hlt");
        }
    }
    
    // Show initial performance stats
    perf_print_stats();
    
    kos_kernel_start();
    
    // This should never return
    while (1) {
        // Update performance metrics in idle loop
        perf_update();
        
        // Check for performance alerts
        perf_alert_t alert = perf_check_alerts();
        if (alert != PERF_ALERT_NONE) {
            log_info("Performance Alert: %s", perf_alert_string(alert));
        }
        
        // Use HAL for idle
        hal_halt();
    }
}
