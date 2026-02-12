#include "perf_monitor.h"
#include "debug/debug.h"
#include "print.h"
#include <string.h>

// Global performance metrics
static perf_metrics_t g_perf_metrics;
static int perf_initialized = 0;

// CPUID helper for RDTSC
static uint64_t rdtsc(void) {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((uint64_t)high << 32) | low;
}

// Get current timestamp (simplified)
static uint64_t get_timestamp(void) {
    return rdtsc();
}

void perf_init(void) {
    if (perf_initialized) return;
    
    // Initialize all counters to zero
    memset(&g_perf_metrics, 0, sizeof(perf_metrics_t));
    
    // Set initial values
    g_perf_metrics.boot_time = get_timestamp();
    g_perf_metrics.last_update_time = g_perf_metrics.boot_time;
    g_perf_metrics.memory_total = 0x200000; // 2MB heap size
    
    perf_initialized = 1;
    
    log_info("Performance monitor initialized");
}

void perf_update(void) {
    if (!perf_initialized) {
        perf_init();
        return;
    }
    
    uint64_t current_time = get_timestamp();
    uint64_t time_delta = current_time - g_perf_metrics.last_update_time;
    
    // Update uptime (simplified conversion)
    g_perf_metrics.uptime_seconds = (current_time - g_perf_metrics.boot_time) / 1000000;
    
    // Update CPU metrics
    g_perf_metrics.cpu_cycles = rdtsc();
    g_perf_metrics.cpu_instructions = g_perf_metrics.cpu_cycles; // Simplified
    
    // Calculate CPU usage (simplified)
    if (time_delta > 0) {
        g_perf_metrics.cpu_usage_percent = (uint32_t)((g_perf_metrics.cpu_cycles % 100));
    }
    
    // Update memory metrics (simplified)
    // For now, use static memory tracking
    static uint64_t memory_used = 0;
    
    // Simulate memory usage changes
    memory_used = (memory_used + 1024) % (g_perf_metrics.memory_total / 2);
    g_perf_metrics.memory_used = memory_used;
    g_perf_metrics.memory_free = g_perf_metrics.memory_total - memory_used;
    g_perf_metrics.memory_usage_percent = 
        (g_perf_metrics.memory_used * 100) / g_perf_metrics.memory_total;
    
    g_perf_metrics.last_update_time = current_time;
}

perf_metrics_t* perf_get_metrics(void) {
    if (!perf_initialized) {
        perf_init();
    }
    
    perf_update();
    return &g_perf_metrics;
}

void perf_print_stats(void) {
    perf_metrics_t* metrics = perf_get_metrics();
    
    printf("\n=== KOS Performance Monitor ===\n");
    printf("Uptime: %d seconds\n", metrics->uptime_seconds);
    
    printf("\nCPU Statistics:\n");
    printf("  Usage: %d%%\n", metrics->cpu_usage_percent);
    printf("  Cycles: %llu\n", metrics->cpu_cycles);
    printf("  Instructions: %llu\n", metrics->cpu_instructions);
    
    printf("\nMemory Statistics:\n");
    printf("  Total: %llu bytes\n", metrics->memory_total);
    printf("  Used: %llu bytes (%d%%)\n", metrics->memory_used, metrics->memory_usage_percent);
    printf("  Free: %llu bytes\n", metrics->memory_free);
    
    printf("\nSystem Statistics:\n");
    printf("  Interrupts: %d\n", metrics->interrupts_count);
    printf("  Context Switches: %d\n", metrics->context_switches);
    
    printf("\nI/O Statistics:\n");
    printf("  Disk Reads: %d\n", metrics->disk_reads);
    printf("  Disk Writes: %d\n", metrics->disk_writes);
    printf("  Network Packets: %d\n", metrics->network_packets);
    
    // Check for performance alerts
    perf_alert_t alert = perf_check_alerts();
    if (alert != PERF_ALERT_NONE) {
        printf("\nALERT: %s\n", perf_alert_string(alert));
    }
    
    printf("===============================\n\n");
}

void perf_reset_counters(void) {
    if (!perf_initialized) return;
    
    g_perf_metrics.cpu_cycles = 0;
    g_perf_metrics.cpu_instructions = 0;
    g_perf_metrics.cpu_cache_hits = 0;
    g_perf_metrics.cpu_cache_misses = 0;
    g_perf_metrics.interrupts_count = 0;
    g_perf_metrics.context_switches = 0;
    g_perf_metrics.disk_reads = 0;
    g_perf_metrics.disk_writes = 0;
    g_perf_metrics.network_packets = 0;
    
    log_info("Performance counters reset");
}

uint64_t perf_get_cycles(void) {
    return rdtsc();
}

uint64_t perf_get_timestamp(void) {
    return get_timestamp();
}

void perf_log_timing(const char* name, uint64_t cycles) {
    log_info("Timing [%s]: %llu cycles", name, cycles);
}

perf_alert_t perf_check_alerts(void) {
    perf_metrics_t* metrics = perf_get_metrics();
    
    if (metrics->cpu_usage_percent > PERF_CPU_HIGH_USAGE) {
        return PERF_ALERT_CPU_HIGH;
    }
    
    if (metrics->memory_usage_percent > PERF_MEM_HIGH_USAGE) {
        return PERF_ALERT_MEMORY_HIGH;
    }
    
    if (metrics->interrupts_count > PERF_INTERRUPT_THRESHOLD) {
        return PERF_ALERT_INTERRUPT_STORM;
    }
    
    return PERF_ALERT_NONE;
}

const char* perf_alert_string(perf_alert_t alert) {
    switch (alert) {
        case PERF_ALERT_CPU_HIGH:
            return "High CPU usage detected";
        case PERF_ALERT_MEMORY_HIGH:
            return "High memory usage detected";
        case PERF_ALERT_INTERRUPT_STORM:
            return "Interrupt storm detected";
        case PERF_ALERT_IO_BOTTLENECK:
            return "I/O bottleneck detected";
        default:
            return "No alerts";
    }
}

// Hook functions for system integration
void perf_increment_interrupts(void) {
    if (perf_initialized) {
        g_perf_metrics.interrupts_count++;
    }
}

void perf_increment_context_switches(void) {
    if (perf_initialized) {
        g_perf_metrics.context_switches++;
    }
}

void perf_increment_disk_reads(void) {
    if (perf_initialized) {
        g_perf_metrics.disk_reads++;
    }
}

void perf_increment_disk_writes(void) {
    if (perf_initialized) {
        g_perf_metrics.disk_writes++;
    }
}

void perf_increment_network_packets(void) {
    if (perf_initialized) {
        g_perf_metrics.network_packets++;
    }
}
