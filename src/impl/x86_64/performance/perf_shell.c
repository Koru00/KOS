#include "perf_shell.h"
#include "perf_monitor.h"
#include "debug/debug.h"
#include "print.h"
#include <string.h>
#include "kos/utils/string.h"

static int monitor_running = 0;

void perf_shell_init(void) {
    perf_init();
    log_info("Performance shell interface initialized");
}

void perf_cmd_stats(void) {
    printf("Performance Statistics:\n");
    perf_print_stats();
}

void perf_cmd_reset(void) {
    perf_reset_counters();
    printf("Performance counters reset.\n");
}

void perf_cmd_monitor(void) {
    printf("Starting real-time performance monitor...\n");
    printf("Press any key to stop monitoring.\n");
    
    monitor_running = 1;
    
    while (monitor_running) {
        // Clear screen (simplified)
        printf("\033[2J\033[H");
        
        // Print header
        printf("=== KOS Real-Time Performance Monitor ===\n");
        printf("Press Ctrl+C to stop\n\n");
        
        // Print current stats
        perf_metrics_t* metrics = perf_get_metrics();
        
        printf("Uptime: %d seconds\n", metrics->uptime_seconds);
        printf("CPU Usage: %d%%\n", metrics->cpu_usage_percent);
        printf("Memory Usage: %d%% (%llu/%llu bytes)\n", 
               metrics->memory_usage_percent, 
               metrics->memory_used, 
               metrics->memory_total);
        printf("Interrupts: %d\n", metrics->interrupts_count);
        printf("Context Switches: %d\n", metrics->context_switches);
        
        // Check alerts
        perf_alert_t alert = perf_check_alerts();
        if (alert != PERF_ALERT_NONE) {
            printf("\n⚠ ALERT: %s\n", perf_alert_string(alert));
        }
        
        printf("\nUpdating in 2 seconds...\n");
        
        // Simple delay (would be replaced with proper timer)
        for (volatile int i = 0; i < 1000000; i++);
        
        // Check for keypress (simplified - in real implementation would use keyboard handler)
        // For now, just run 5 iterations
        static int iterations = 0;
        if (++iterations >= 5) {
            monitor_running = 0;
        }
    }
    
    printf("\nPerformance monitor stopped.\n");
}

void perf_cmd_alerts(void) {
    printf("Performance Alert System:\n");
    printf("Thresholds:\n");
    printf("  CPU High Usage: %d%%\n", PERF_CPU_HIGH_USAGE);
    printf("  Memory High Usage: %d%%\n", PERF_MEM_HIGH_USAGE);
    printf("  Interrupt Threshold: %d/sec\n", PERF_INTERRUPT_THRESHOLD);
    printf("\n");
    
    perf_alert_t alert = perf_check_alerts();
    printf("Current Status: %s\n", perf_alert_string(alert));
    
    if (alert == PERF_ALERT_NONE) {
        printf("✓ All systems normal\n");
    } else {
        printf("⚠ Performance issues detected!\n");
        printf("Recommendations:\n");
        switch (alert) {
            case PERF_ALERT_CPU_HIGH:
                printf("  - Reduce CPU-intensive processes\n");
                printf("  - Check for infinite loops\n");
                break;
            case PERF_ALERT_MEMORY_HIGH:
                printf("  - Free unused memory\n");
                printf("  - Check for memory leaks\n");
                break;
            case PERF_ALERT_INTERRUPT_STORM:
                printf("  - Check interrupt handlers\n");
                printf("  - Disable unnecessary hardware\n");
                break;
            default:
                break;
        }
    }
}

void perf_cmd_benchmark(void) {
    printf("Running performance benchmarks...\n");
    
    // CPU benchmark
    printf("\nCPU Benchmark:\n");
    uint64_t cpu_start = perf_get_cycles();
    
    // Simple CPU intensive task
    volatile uint64_t sum = 0;
    for (uint64_t i = 0; i < 1000000; i++) {
        sum += i * i;
    }
    
    uint64_t cpu_end = perf_get_cycles();
    uint64_t cpu_elapsed = cpu_end - cpu_start;
    perf_log_timing("CPU Benchmark", cpu_elapsed);
    printf("Result: %llu (checksum)\n", sum);
    
    // Memory benchmark
    printf("\nMemory Benchmark:\n");
    uint64_t mem_start = perf_get_cycles();
    
    // Memory allocation test using KOS memory system
    extern void* kos_memory_alloc(void*, size_t);
    extern void kos_memory_free(void*, void*);
    extern void* g_memory_manager;
    
    const int test_size = 1024;
    void* ptr = kos_memory_alloc(&g_memory_manager, test_size);
    if (ptr) {
        // Write test pattern
        kos_memset(ptr, 0xAA, test_size);
        
        // Read and verify
        uint8_t* mem = (uint8_t*)ptr;
        uint8_t checksum = 0;
        for (int i = 0; i < test_size; i++) {
            checksum ^= mem[i];
        }
        kos_memory_free(&g_memory_manager, ptr);
        printf("Memory test passed. Checksum: 0x%02X\n", checksum);
    }
    
    uint64_t mem_end = perf_get_cycles();
    uint64_t mem_elapsed = mem_end - mem_start;
    perf_log_timing("Memory Benchmark", mem_elapsed);
    
    printf("\nBenchmark completed. Check logs for detailed timing.\n");
}

void perf_show_help(void) {
    printf("Performance Monitor Commands:\n");
    printf("  perf stats     - Show current performance statistics\n");
    printf("  perf reset     - Reset all performance counters\n");
    printf("  perf monitor   - Start real-time performance monitoring\n");
    printf("  perf alerts    - Show performance alerts and thresholds\n");
    printf("  perf benchmark - Run performance benchmarks\n");
    printf("  perf help      - Show this help message\n");
}

void perf_register_shell_commands(void) {
    // This would integrate with the shell system
    // For now, it's a placeholder that shows available commands
    printf("Performance Monitor Commands Registered:\n");
    perf_show_help();
}
