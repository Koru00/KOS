# KOS Performance Monitoring System

## Overview
KOS includes a comprehensive performance monitoring system that tracks CPU usage, memory consumption, system metrics, and I/O operations without requiring a traditional task manager.

## Features

### Real-Time Performance Metrics
- **CPU Usage**: Cycle counting and instruction tracking
- **Memory Usage**: Heap allocation monitoring with usage percentages
- **System Metrics**: Uptime, interrupt count, context switches
- **I/O Statistics**: Disk operations and network packet tracking

### Performance Alerts
- Automatic detection of performance issues
- Configurable thresholds for CPU and memory usage
- Interrupt storm detection
- I/O bottleneck warnings

### Built-in Benchmarks
- CPU performance testing
- Memory allocation benchmarks
- Timing measurements with RDTSC

## Usage

### Automatic Monitoring
The performance monitor starts automatically when KOS boots:
```c
// In kernel_main()
perf_init();           // Initialize monitoring
perf_update();         // Update metrics (called in idle loop)
perf_check_alerts();   // Check for performance issues
```

### Manual Commands (when shell is available)
```
perf stats      - Show current performance statistics
perf reset      - Reset all performance counters
perf monitor    - Start real-time performance monitoring
perf alerts     - Show performance alerts and thresholds
perf benchmark  - Run performance benchmarks
```

### External Monitoring Tools

#### Real-time Viewer
```bash
.\perf-view.bat
```
Continuously monitors the debug log and shows real-time performance data.

#### Performance Reports
```bash
.\perf-report.bat
```
Generates detailed performance reports with analysis and recommendations.

## Performance Metrics

### CPU Metrics
- **Cycles**: CPU cycles counted via RDTSC
- **Instructions**: Approximate instruction count
- **Usage Percentage**: Simplified CPU usage calculation
- **Cache Hits/Misses**: Placeholder for future implementation

### Memory Metrics
- **Total**: Total heap size (2MB default)
- **Used**: Currently allocated memory
- **Free**: Available memory
- **Usage Percentage**: Memory utilization

### System Metrics
- **Uptime**: System running time in seconds
- **Interrupts**: Total interrupt count
- **Context Switches**: Task switching count

### I/O Metrics
- **Disk Reads**: Number of read operations
- **Disk Writes**: Number of write operations
- **Network Packets**: Network packet count

## Performance Alerts

### Alert Types
- `PERF_ALERT_CPU_HIGH`: CPU usage > 80%
- `PERF_ALERT_MEMORY_HIGH`: Memory usage > 85%
- `PERF_ALERT_INTERRUPT_STORM`: Too many interrupts
- `PERF_ALERT_IO_BOTTLENECK`: I/O performance issues

### Threshold Configuration
```c
#define PERF_CPU_HIGH_USAGE    80
#define PERF_MEM_HIGH_USAGE    85
#define PERF_INTERRUPT_THRESHOLD 1000
```

## Integration Examples

### Adding Performance Hooks
```c
// In interrupt handler
void interrupt_handler(void) {
    perf_increment_interrupts();
    // ... handle interrupt
}

// In memory allocation
void* my_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        perf_update();  // Update memory metrics
    }
    return ptr;
}
```

### Custom Performance Measurements
```c
// Measure function execution time
void my_function(void) {
    uint64_t start = perf_get_cycles();
    
    // ... function code
    
    uint64_t end = perf_get_cycles();
    perf_log_timing("my_function", end - start);
}
```

## Data Output

### Serial Logging
All performance data is logged to the serial port and saved to `debug_log/debug_log.txt`.

### Log Format
```
[INFO] KOS Kernel Starting...
[INFO] Performance Monitor Active
[INFO] Timing [CPU Benchmark]: 1234567 cycles
[INFO] Performance Alert: High CPU usage detected
```

### Performance Report Sample
```
KOS Performance Report
Generated: 2024-02-08 22:59:00

Performance Metrics:
CPU Usage: 45%
Memory Usage: 23% (4718592/20971520 bytes)
Interrupts: 1247
Context Switches: 89

Performance Summary:
Total Log Entries: 156
Performance Alerts: 1
Timing Measurements: 3

Recommendations:
- Performance alerts detected. Review system load.
```

## Technical Implementation

### Timing
- Uses RDTSC instruction for high-resolution timing
- Provides cycle-accurate measurements
- Timestamp tracking for performance analysis

### Memory Tracking
- Integrates with the heap allocator
- Tracks allocation patterns
- Provides usage statistics

### System Integration
- Hooks into kernel idle loop
- Interrupt counting integration
- Minimal performance overhead

## Future Enhancements

### Planned Features
- Per-process performance tracking
- Network performance metrics
- Disk I/O performance analysis
- GUI performance monitor
- Historical data tracking

### Performance Optimizations
- Reduced overhead measurements
- More accurate CPU usage calculation
- Enhanced memory tracking
- Cache performance analysis

## Troubleshooting

### Common Issues
1. **No performance data**: Ensure performance monitor is initialized
2. **Inaccurate metrics**: Check timer calibration
3. **Missing alerts**: Verify threshold configuration
4. **High overhead**: Reduce monitoring frequency

### Debug Information
Performance data is automatically logged and can be analyzed with the provided tools.

## Configuration

### Compile-Time Settings
- Memory tracking can be disabled for minimal overhead
- Alert thresholds are configurable
- Monitoring frequency can be adjusted

### Runtime Configuration
- Performance counters can be reset
- Monitoring can be enabled/disabled
- Alert thresholds can be adjusted
