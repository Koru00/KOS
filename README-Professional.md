# KOS - Professional Kernel Operating System

## Overview
KOS is a professional x86_64 kernel operating system with advanced performance monitoring, debugging capabilities, and development tools.

## Quick Start

### Build and Run
```bash
# Professional build system
.\build-professional.bat

# Professional launcher
.\run-professional.bat
```

### Performance Monitoring
```bash
# Performance monitoring suite
.\tools\performance-monitor.bat

# Debug tools
.\tools\debug-tools.bat
```

## Project Structure

```
KOS/
├── src/
│   ├── impl/           # Implementation files
│   │   ├── kernel/     # Core kernel
│   │   ├── kos/        # KOS system
│   │   └── x86_64/     # Architecture-specific
│   └── intf/           # Interface headers
│       ├── performance/ # Performance monitoring
│       ├── debug/      # Debug system
│       └── drivers/    # Driver interfaces
├── tools/              # Professional tools
├── debug_log/          # Debug output
├── reports/            # Performance reports
└── exports/            # Data exports
```

## Professional Tools

### Build System (`build-professional.bat`)
- **Standard Build**: Optimized release build
- **Debug Build**: Debug symbols and logging
- **Clean Build**: Remove all artifacts
- **Build and Run**: Complete build and launch
- **Performance Build**: Performance-optimized build

### Launcher (`run-professional.bat`)
- **Standard Mode**: Normal operation with display
- **Debug Mode**: GDB debugging support
- **Benchmark Mode**: Headless performance testing
- **Performance Mode**: Real-time monitoring
- **Custom Mode**: Custom QEMU parameters

### Performance Monitor (`tools/performance-monitor.bat`)
- **Real-time Monitor**: Live performance tracking
- **Performance Report**: Detailed analysis reports
- **Quick Analysis**: Fast performance overview
- **Benchmark Test**: Automated performance testing
- **Export Data**: Data export functionality

### Debug Tools (`tools/debug-tools.bat`)
- **GDB Session**: Full kernel debugging
- **QEMU Monitor**: Hardware monitoring
- **Log Analysis**: Debug log examination
- **Memory Analysis**: Memory usage and layout
- **Register Analysis**: CPU register examination

## Performance Monitoring System

### Real-time Metrics
- CPU usage and cycle counting
- Memory allocation tracking
- System interrupt monitoring
- I/O operation statistics

### Performance Alerts
- Automatic threshold detection
- CPU high usage warnings (>80%)
- Memory pressure alerts (>85%)
- Interrupt storm detection

### Benchmark Suite
- CPU performance testing
- Memory allocation benchmarks
- System responsiveness testing
- Automated report generation

## Development Features

### Professional Debugging
- GDB integration with remote debugging
- QEMU monitor for hardware inspection
- Comprehensive logging system
- Memory and register analysis tools

### Build System
- Docker-based cross-compilation
- Multiple build configurations
- Automated dependency tracking
- Professional error reporting

### Performance Analysis
- Real-time performance monitoring
- Historical data tracking
- Automated report generation
- Performance regression detection

## Architecture

### Kernel Components
- **Boot System**: Multiboot2 compliant bootloader
- **Memory Management**: Heap allocator with tracking
- **Interrupt System**: IDT/IRQ/ISR handling
- **Driver System**: VGA, keyboard, serial drivers
- **File System**: Basic file operations
- **Performance System**: Built-in monitoring

### Hardware Support
- x86_64 architecture
- QEMU virtualization
- VGA graphics
- PS/2 keyboard
- Serial communication
- Intel Q35 chipset emulation

## Configuration

### Build Configuration
```bash
# Standard build
make

# Debug build  
make DEBUG=1

# Performance build
make PERFORMANCE=1
```

### QEMU Configuration
```bash
# Standard mode
qemu-system-x86_64 -cpu qemu64 -smp 4 -m 2048 -machine q35

# Debug mode
qemu-system-x86_64 -s -S -d int,cpu_reset

# Benchmark mode
qemu-system-x86_64 -display none -vga none
```

## Performance Monitoring

### Metrics Collection
The system automatically collects:
- CPU cycles and instructions
- Memory allocation patterns
- Interrupt frequency and types
- I/O operation counts
- System uptime and load

### Alert System
Automatic detection of:
- High CPU utilization
- Memory pressure situations
- Interrupt storms
- Performance regressions

### Reporting
- Real-time performance dashboards
- Historical trend analysis
- Automated performance reports
- Export capabilities for analysis

## Debugging

### GDB Debugging
```bash
# Connect to GDB server
gdb-multiarch -ex "target remote localhost:1234"

# Common commands
break kernel_main
continue
info registers
step
```

### QEMU Monitor
```bash
# Connect to monitor
telnet localhost 1234

# Common commands
info registers
info mem
x/100wx 0x100000
```

## Contributing

### Development Workflow
1. Use `build-professional.bat` for builds
2. Test with `run-professional.bat`
3. Monitor performance with `tools/performance-monitor.bat`
4. Debug issues with `tools/debug-tools.bat`

### Code Style
- English comments and documentation
- Consistent naming conventions
- Professional error handling
- Comprehensive logging

## Documentation

- [Performance Monitoring](README-Performance.md)
- [QEMU Configuration](README-QEMU.md)
- [Debug Guide](docs/DEBUG.md)
- [API Reference](docs/API.md)

## License

Professional kernel operating system for educational and research purposes.

## Support

For issues and questions:
1. Check debug logs in `debug_log/`
2. Run performance analysis tools
3. Review generated reports
4. Use debug tools for investigation
