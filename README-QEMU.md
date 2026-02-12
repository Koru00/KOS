# KOS Professional QEMU Setup

## Overview
This document describes the professional QEMU configuration for KOS (Kernel Operating System) development.

## Quick Start

### 1. Standard Mode (Recommended for development)
```bash
.\run-pro.bat
# Select option 1
```

### 2. Debug Mode (For kernel debugging)
```bash
.\run-pro.bat
# Select option 2
# Then run: .\qemu-debug.bat
```

### 3. Benchmark Mode (Performance testing)
```bash
.\run-pro.bat
# Select option 3
```

## Features

### Professional Hardware Emulation
- **CPU**: Host CPU with full virtualization (KVM)
- **Chipset**: Intel Q35 (modern chipset)
- **Graphics**: VirtIO VGA (high performance)
- **Storage**: VirtIO SCSI (better performance than IDE)
- **Network**: Intel E1000 with user networking
- **Audio**: Intel HDA audio
- **Random**: VirtIO RNG (hardware random number generator)
- **Memory**: VirtIO balloon (dynamic memory management)

### Debug Features
- **GDB Support**: Full kernel debugging with breakpoints
- **Monitor**: QEMU monitor on telnet localhost:1234
- **Serial Logging**: All kernel output logged to `debug_log/debug_log.txt`
- **Trace Logging**: Detailed QEMU traces in debug mode

### Performance Optimizations
- **KVM Acceleration**: Hardware virtualization for maximum performance
- **VirtIO Drivers**: Paravirtualized drivers for better I/O performance
- **CPU Features**: Host CPU features with proper checking
- **Memory Management**: Optimized memory allocation and handling

## Configuration Files

### Make Targets
```bash
make run        # Standard professional mode
make debug      # Debug mode with GDB support
make benchmark  # Headless benchmark mode
```

### Batch Scripts
- `run-pro.bat` - Interactive launcher with multiple modes
- `qemu-debug.bat` - GDB debugging helper
- `qemu-monitor.bat` - QEMU monitor connector
- `build-only.bat` - Build without launching QEMU

## Debugging

### GDB Debugging
1. Launch QEMU in debug mode: `.\run-pro.bat` → option 2
2. Start GDB: `.\qemu-debug.bat`
3. Use GDB commands:
   - `break kernel_main` - Set breakpoint at kernel entry
   - `continue` - Start execution
   - `step` - Step through code
   - `info registers` - View CPU state
   - `x/10i $pc` - View assembly around program counter

### Monitor Commands
Connect with: `.\qemu-monitor.bat`
- `info registers` - Show CPU registers
- `x/10i $pc` - Examine memory
- `quit` - Exit monitor

## Hardware Configuration

### CPU Settings
- Host CPU with full feature set
- 4 cores (configurable via QEMU_CORES)
- 2GB RAM (configurable via QEMU_RAM)
- KVM acceleration for maximum performance

### Storage
- VirtIO SCSI controller
- CD-ROM drive with kernel ISO
- High-performance storage emulation

### Network
- Intel E1000 network card
- User networking (NAT)
- Internet access from guest

### Audio
- Intel HDA audio controller
- Duplex audio device

## Logging and Monitoring

### Serial Output
- All kernel messages logged to `debug_log/debug_log.txt`
- Real-time kernel debugging output

### QEMU Traces
- Detailed traces in debug mode: `debug_log/qemu_trace.log`
- CPU and interrupt tracing for debugging

### Monitor Access
- Telnet interface on localhost:1234
- Real-time system monitoring and control

## Performance Tuning

### For Maximum Performance
```bash
make benchmark
# Uses -machine q35,accel=kvm
# Headless mode for maximum CPU utilization
```

### For Development
```bash
make run
# Balanced performance with display
# Full debugging features enabled
```

### For Debugging
```bash
make debug
# Full debugging with GDB support
# Detailed tracing enabled
```

## Troubleshooting

### Common Issues
1. **KVM not available**: QEMU will fall back to software emulation
2. **Permission denied**: Make sure user has access to /dev/kvm
3. **Missing QEMU**: Install from https://qemu.weilnetz.de/w64/
4. **GDB not found**: Install from https://winlibs.com/

### Performance Tips
- Ensure KVM is available for best performance
- Use VirtIO drivers when possible
- Disable unused devices for better performance
- Use benchmark mode for performance testing

### Debug Tips
- Use debug mode for kernel development
- Monitor serial output for kernel messages
- Use GDB breakpoints for code debugging
- Check QEMU traces for hardware issues
