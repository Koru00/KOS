# KOS Run System

## Overview
The KOS run system provides multiple execution modes for development, debugging, and performance testing.

## Usage

### Basic Run
```bash
run.bat
```

## Run Modes

### 1. Standard Mode
Default execution mode with display output.

**Features:**
- VGA graphics enabled
- Serial logging enabled
- QEMU monitor accessible
- Standard memory and CPU configuration

**QEMU Parameters:**
```bash
qemu-system-x86_64 \
    -cpu qemu64 \
    -smp 4 \
    -m 2048 \
    -machine q35 \
    -vga std \
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom \
    -serial file:debug_log/debug_log.txt \
    -monitor telnet:localhost:1234,server,nowait \
    -boot d
```

### 2. Debug Mode
Enhanced debugging with GDB support.

**Features:**
- GDB server on port 1234
- CPU and interrupt tracing
- Detailed QEMU logging
- Paused at startup for debugger attachment

**QEMU Parameters:**
```bash
qemu-system-x86_64 \
    -cpu qemu64 \
    -smp 4 \
    -m 2048 \
    -machine q35 \
    -vga std \
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom \
    -serial file:debug_log/debug_log.txt \
    -monitor telnet:localhost:1234,server,nowait \
    -s -S \
    -d int,cpu_reset \
    -D debug_log/qemu_trace.log \
    -boot d
```

**GDB Connection:**
```bash
gdb-multiarch -ex "target remote localhost:1234"
```

### 3. Benchmark Mode
Headless mode for performance testing.

**Features:**
- No display output (headless)
- Optimized for performance
- Serial logging only
- Maximum resource allocation

**QEMU Parameters:**
```bash
qemu-system-x86_64 \
    -cpu qemu64 \
    -smp 4 \
    -m 2048 \
    -machine q35 \
    -display none \
    -vga none \
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom \
    -serial file:debug_log/debug_log.txt \
    -monitor telnet:localhost:1234,server,nowait \
    -boot d
```

## Configuration

### Hardware Emulation
- **CPU**: qemu64 (compatible with all systems)
- **Cores**: 4 virtual CPUs
- **Memory**: 2048MB RAM
- **Chipset**: Intel Q35 (modern)
- **Graphics**: VGA standard
- **Storage**: IDE CD-ROM

### Debug Features
- **Serial Port**: File logging to `debug_log/debug_log.txt`
- **Monitor**: Telnet interface on localhost:1234
- **GDB**: Remote debugging support
- **Tracing**: CPU and interrupt tracing

## Monitoring

### Serial Output
All kernel output is logged to `debug_log/debug_log.txt` for analysis.

### QEMU Monitor
Connect via telnet for hardware inspection:
```bash
telnet localhost 1234
```

Common monitor commands:
- `info registers` - Show CPU registers
- `info mem` - Show memory map
- `x/100wx 0x100000` - Examine memory
- `quit` - Exit monitor

### GDB Debugging
For detailed kernel debugging:
```bash
# Connect GDB
gdb-multiarch dist/x86_64/kernel.bin

# Set target
(gdb) target remote localhost:1234

# Set breakpoints
(gdb) break kernel_main

# Start execution
(gdb) continue
```

## Troubleshooting

### Common Issues

#### QEMU Not Found
```bash
# Install QEMU
# Windows: Download from qemu.weilnetz.de
# Linux: sudo apt install qemu-system-x86
```

#### ISO Not Found
```bash
# Build first
./build.bat
```

#### Permission Denied
```bash
# Check Docker permissions
sudo usermod -aG docker $USER
# Restart Docker
```

#### Display Issues
Try different VGA modes:
- `-vga std` (standard)
- `-vga cirrus` (compatible)
- `-vga none` (headless)

### Performance Issues

#### Slow Emulation
- Enable KVM if available: `-enable-kvm`
- Use host CPU: `-cpu host`
- Increase memory: `-m 4096`

#### Graphics Problems
- Try different VGA adapters
- Use `-vga none` for headless
- Check QEMU version compatibility

## Advanced Usage

### Custom Parameters
For testing specific configurations:
```bash
qemu-system-x86_64 \
    -cpu qemu64 \
    -smp 4 \
    -m 2048 \
    -machine q35 \
    [custom parameters] \
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom \
    -boot d
```

### Network Configuration
Add networking support:
```bash
-netdev user,id=net0 -device e1000,netdev=net0
```

### Additional Devices
Add virtual hardware:
```bash
-device virtio-rng-pci \      # Random number generator
-device virtio-balloon-pci \   # Memory balloon
-soundhw hda               # Audio support
```

## Integration

### with Build System
The run system automatically checks for ISO existence and offers to build if missing.

### with Performance Monitor
Serial output integrates with performance monitoring tools for real-time analysis.

### with Debug Tools
Debug mode provides comprehensive integration with GDB and QEMU monitor.
