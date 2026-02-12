# KOS Performance Monitoring System

## Overview
KOS includes a comprehensive performance monitoring system that tracks CPU usage, memory consumption, and system metrics without requiring external task managers.

## Features

### Real-time Metrics
- CPU usage and cycle counting
- Memory allocation tracking
- System interrupt monitoring
- I/O operation statistics
- Performance alerts and thresholds

### Built-in Analysis
- Automated performance reports
- Benchmark testing suite
- Historical data tracking
- Performance regression detection

## Usage

### Performance Monitor
```bash
perf.bat
```

### Monitor Options

#### 1. View Performance Data
Displays current performance metrics and statistics.

#### 2. Performance Report
Generates detailed performance analysis reports with timestamps.

#### 3. Real-time Monitor
Continuous monitoring with automatic updates every 3 seconds.

## Performance Metrics

### CPU Metrics
- **Usage Percentage**: Current CPU utilization
- **Cycle Count**: Total CPU cycles executed
- **Instructions**: Approximate instruction count
- **Cache Performance**: Hit/miss ratios (future)

### Memory Metrics
- **Total Memory**: System memory size (2MB default)
- **Used Memory**: Currently allocated memory
- **Free Memory**: Available memory
- **Usage Percentage**: Memory utilization

### System Metrics
- **Uptime**: System running time in seconds
- **Interrupts**: Total interrupt count
- **Context Switches**: Task switching frequency
- **I/O Operations**: Disk and network activity

## Performance Alerts

### Alert Types
- `PERF_ALERT_CPU_HIGH`: CPU usage > 80%
- `PERF_ALERT_MEMORY_HIGH`: Memory usage > 85%
- `PERF_ALERT_INTERRUPT_STORM`: Excessive interrupts
- `PERF_ALERT_IO_BOTTLENECK`: I/O performance issues

### Thresholds
```c
#define PERF_CPU_HIGH_USAGE    80
#define PERF_MEM_HIGH_USAGE    85
#define PERF_INTERRUPT_THRESHOLD 1000
```

## Integration

### Kernel Integration
The performance monitor is automatically initialized in `kernel_main()`:

```c
void kernel_main(void) {
    perf_init();              // Initialize monitoring
    perf_shell_init();         // Setup shell commands
    
    // ... kernel initialization
    
    while (1) {
        perf_update();          // Update metrics
        perf_check_alerts();   // Check for issues
        asm volatile("hlt");
    }
}
```

### Hook Functions
For custom performance tracking:
```c
// Increment counters in system code
perf_increment_interrupts();      // In interrupt handlers
perf_increment_context_switches(); // In task switcher
perf_increment_disk_reads();       // In disk I/O
perf_increment_network_packets();  // In network stack
```

### Performance Measurement
For timing critical operations:
```c
// Measure execution time
uint64_t start = perf_get_cycles();

// ... operation to measure ...

uint64_t end = perf_get_cycles();
perf_log_timing("operation_name", end - start);
```

## Data Output

### Serial Logging
All performance data is logged to serial port and saved to `debug_log/debug_log.txt`.

### Log Format
```
[INFO] KOS Kernel Starting...
[INFO] Performance Monitor Active
[INFO] Timing [CPU Benchmark]: 1234567 cycles
[INFO] Performance Alert: High CPU usage detected
=== KOS Performance Monitor ===
Uptime: 45 seconds
CPU Statistics:
  Usage: 67%
  Cycles: 1234567890
Memory Statistics:
  Total: 2097152 bytes
  Used: 1048576 bytes (50%)
System Statistics:
  Interrupts: 1247
  Context Switches: 89
```

## Reports

### Performance Reports
Generated with timestamp and comprehensive analysis:
```
KOS Performance Report
Generated: 2024-02-08 23:30:00
========================================

Performance Metrics:
CPU Usage: 67%
Memory Usage: 50% (1048576/2097152 bytes)
Interrupts: 1247
Context Switches: 89

Performance Summary:
Total Log Entries: 156
Performance Alerts: 1
Timing Measurements: 3

Recommendations:
- Performance alerts detected. Review system load.
```

## Benchmarking

### CPU Benchmark
Tests computational performance:
```c
// CPU intensive task
volatile uint64_t sum = 0;
for (uint64_t i = 0; i < 1000000; i++) {
    sum += i * i;
}
```

### Memory Benchmark
Tests memory allocation and access:
```c
// Memory allocation test
void* ptr = malloc(1024);
memset(ptr, 0xAA, 1024);

// Memory verification
volatile uint8_t* mem = (uint8_t*)ptr;
uint8_t checksum = 0;
for (int i = 0; i < 1024; i++) {
    checksum ^= mem[i];
}
```

## Troubleshooting

### No Performance Data
1. Ensure performance monitor is initialized
2. Check `debug_log/debug_log.txt` exists
3. Verify kernel is running with monitoring enabled

### Inaccurate Metrics
1. Check timer calibration
2. Verify RDTSC implementation
3. Update performance thresholds

### High Overhead
1. Reduce monitoring frequency
2. Disable unused metrics
3. Optimize performance code

## Configuration

### Compile-time Settings
```c
// Disable performance monitoring
#define PERF_DISABLED

// Custom thresholds
#define PERF_CPU_THRESHOLD 75
#define PERF_MEM_THRESHOLD 90
```

### Runtime Configuration
```c
// Reset counters
perf_reset_counters();

// Get current metrics
perf_metrics_t* metrics = perf_get_metrics();

// Print statistics
perf_print_stats();
```

## Future Enhancements

### Planned Features
- Per-process performance tracking
- Network performance metrics
- Disk I/O performance analysis
- GUI performance dashboard
- Historical trend analysis

### Performance Optimizations
- Reduced overhead measurements
- More accurate CPU usage calculation
- Enhanced memory tracking
- Cache performance analysis

## API Reference

### Core Functions
```c
void perf_init(void);
void perf_update(void);
perf_metrics_t* perf_get_metrics(void);
void perf_print_stats(void);
void perf_reset_counters(void);
```

### Measurement Functions
```c
uint64_t perf_get_cycles(void);
uint64_t perf_get_timestamp(void);
void perf_log_timing(const char* name, uint64_t cycles);
```

### Alert Functions
```c
perf_alert_t perf_check_alerts(void);
const char* perf_alert_string(perf_alert_t alert);
```

### Hook Functions
```c
void perf_increment_interrupts(void);
void perf_increment_context_switches(void);
void perf_increment_disk_reads(void);
void perf_increment_disk_writes(void);
void perf_increment_network_packets(void);
```
