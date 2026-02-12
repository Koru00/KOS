# KOS - Kernel Operating System

A lightweight, experimental x86_64 operating system written in C and assembly.  
**KOS** is designed to explore low-level system architecture with professional development tools.

---

## Quick Start

### Build System
```bash
build.bat
```
Build options:
- [1] Standard Build
- [2] Debug Build  
- [3] Clean Build
- [4] Build and Run

### Run System
```bash
run.bat
```
Run options:
- [1] Standard Mode
- [2] Debug Mode
- [3] Benchmark Mode

### Performance Monitor
```bash
perf.bat
```
Performance options:
- [1] View Performance Data
- [2] Performance Report
- [3] Real-time Monitor

### Debug Tools
```bash
debug.bat
```
Debug options:
- [1] GDB Debug Session
- [2] QEMU Monitor
- [3] View Debug Log

### Cleanup
```bash
clean.bat
```
Cleanup options:
- [1] Clean Build Artifacts
- [2] Clean Debug Logs
- [3] Clean Everything

---

## Documentation

- [Build System](docs/BUILD.md) - Complete build documentation
- [Run System](docs/RUN.md) - Execution modes and configuration
- [Performance Monitoring](docs/PERFORMANCE.md) - Performance system guide
- [Debug Tools](docs/DEBUG.md) - Debugging reference

---

## Features

### Core System
- x86_64 kernel with multiboot2 support
- Memory management with heap allocator
- Interrupt system (IDT/IRQ/ISR)
- VGA graphics and keyboard drivers
- Basic file system operations

### Performance Monitoring
- Real-time CPU and memory tracking
- Performance alerts and thresholds
- Automated benchmarking
- Detailed performance reports

### Debug Tools
- GDB integration for kernel debugging
- QEMU monitor for hardware inspection
- Comprehensive logging system
- Memory and register analysis

---

## Requirements

- [Docker](https://www.docker.com/) - for building
- [QEMU](https://www.qemu.org/) - for emulation

---

## Project Structure

```
KOS/
├── src/
│   ├── impl/           # Implementation files
│   └── intf/           # Interface headers
├── docs/              # Documentation
├── debug_log/          # Debug output
├── build.bat           # Build system
├── run.bat             # Run system
├── perf.bat            # Performance monitor
├── debug.bat           # Debug tools
├── clean.bat           # Cleanup utility
└── README.md           # This file
```

---

## Development Workflow

1. **Build**: `build.bat` → Select build type
2. **Run**: `run.bat` → Select run mode  
3. **Monitor**: `perf.bat` → View performance
4. **Debug**: `debug.bat` → Debug issues
5. **Clean**: `clean.bat` → Clean artifacts

---

## Performance Monitoring

The system includes built-in performance monitoring:
- CPU usage and cycle counting
- Memory allocation tracking
- System interrupt monitoring
- Real-time alerts and reporting

---

## Debugging

Professional debugging capabilities:
- Remote GDB debugging
- QEMU monitor access
- Comprehensive logging
- Memory analysis tools

---

## License

Educational and research use only.
