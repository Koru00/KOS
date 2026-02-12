#pragma once

#include <stdint.h>
#include <stddef.h>

// Performance metrics structure
typedef struct {
    // CPU metrics
    uint64_t cpu_cycles;
    uint64_t cpu_instructions;
    uint64_t cpu_cache_hits;
    uint64_t cpu_cache_misses;
    uint32_t cpu_usage_percent;
    
    // Memory metrics
    uint64_t memory_total;
    uint64_t memory_used;
    uint64_t memory_free;
    uint32_t memory_usage_percent;
    
    // System metrics
    uint32_t uptime_seconds;
    uint32_t interrupts_count;
    uint32_t context_switches;
    
    // I/O metrics
    uint32_t disk_reads;
    uint32_t disk_writes;
    uint32_t network_packets;
    
    // Performance counters
    uint64_t boot_time;
    uint64_t last_update_time;
} perf_metrics_t;

// Performance monitoring functions
void perf_init(void);
void perf_update(void);
perf_metrics_t* perf_get_metrics(void);
void perf_print_stats(void);
void perf_reset_counters(void);

// Performance measurement macros
#define PERF_START_TIMER() uint64_t start_time = perf_get_cycles()
#define PERF_END_TIMER(name) do { \
    uint64_t end_time = perf_get_cycles(); \
    uint64_t elapsed = end_time - start_time; \
    perf_log_timing(name, elapsed); \
} while(0)

// Low-level timing functions
uint64_t perf_get_cycles(void);
uint64_t perf_get_timestamp(void);
void perf_log_timing(const char* name, uint64_t cycles);

// Performance thresholds
#define PERF_CPU_HIGH_USAGE    80
#define PERF_MEM_HIGH_USAGE    85
#define PERF_INTERRUPT_THRESHOLD 1000

// Performance alerts
typedef enum {
    PERF_ALERT_NONE,
    PERF_ALERT_CPU_HIGH,
    PERF_ALERT_MEMORY_HIGH,
    PERF_ALERT_INTERRUPT_STORM,
    PERF_ALERT_IO_BOTTLENECK
} perf_alert_t;

perf_alert_t perf_check_alerts(void);
const char* perf_alert_string(perf_alert_t alert);
