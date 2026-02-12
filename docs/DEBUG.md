# KOS Debug System

## Overview
KOS provides comprehensive debugging capabilities including GDB integration, QEMU monitor access, and detailed logging for kernel development.

## Debug Tools

### Debug Launcher
```bash
debug.bat
```

### Debug Options

#### 1. GDB Debug Session
Full kernel debugging with GDB integration.

**Features:**
- Remote GDB server on port 1234
- Automatic breakpoint at kernel_main
- Symbol loading and backtrace support
- Register and memory inspection

**Usage:**
```bash
# Connect GDB
gdb-multiarch -ex "target remote localhost:1234"

# Common GDB commands
(gdb) break kernel_main
(gdb) continue
(gdb) info registers
(gdb) step
(gdb) backtrace
(gdb) x/10i $pc
```

#### 2. QEMU Monitor
Hardware-level debugging and inspection.

**Features:**
- Real-time hardware monitoring
- Memory inspection and modification
- CPU register viewing
- Device status monitoring

**Usage:**
```bash
# Connect to monitor
telnet localhost 1234

# Common monitor commands
(qemu) info registers
(qemu) info mem
(qemu) x/100wx 0x100000
(qemu) xp/100x $esp
(qemu) quit
```

#### 3. View Debug Log
Analysis of kernel debug output and error messages.

**Features:**
- Real-time log viewing
- Error pattern detection
- Performance data analysis
- Log filtering and search

## Debug Configuration

### GDB Setup
The debug system automatically creates GDB scripts:
```gdb
target remote localhost:1234
set architecture i386:x86-64
set print pretty on
set print array on
set print demangle on
handle SIGUSR1 nostop noprint pass
handle SIGUSR2 nostop noprint pass
info registers
break kernel_main
continue
```

### QEMU Monitor Setup
Monitor configuration includes:
- Telnet server on localhost:1234
- Full hardware access
- Real-time monitoring
- Command history

## Debugging Techniques

### Kernel Entry Point Debugging
```bash
# 1. Start in debug mode
./run.bat -> option 2

# 2. Connect GDB
./debug.bat -> option 1

# 3. Set breakpoints
(gdb) break kernel_main
(gdb) continue

# 4. Step through initialization
(gdb) step
(gdb) next
```

### Memory Debugging
```gdb
# Examine memory regions
(gdb) x/100wx 0x100000    # Kernel code
(gdb) x/100wx 0x200000    # Kernel data
(gdb) x/100wx 0x300000    # Heap area

# Search for patterns
(gdb) find /w 0x100000 0x500000 "KOS"
(gdb) find /b 0x100000 0x500000 0xAA
```

### Register Analysis
```gdb
# View all registers
(gdb) info registers

# View specific registers
(gdb) info registers eip    # Instruction pointer
(gdb) info registers esp    # Stack pointer
(gdb) info registers ebp    # Base pointer
(gdb) info flags          # Flags register

# View flags in detail
(gdb) info registers eflags
```

### Interrupt Debugging
```gdb
# Set breakpoints on interrupt handlers
(gdb) break irq_handler
(gdb) break isr_timer
(gdb) break isr_keyboard

# Continue to interrupt
(gdb) continue
# When interrupt occurs, examine stack
(gdb) backtrace
(gdb) info registers
```

## Log Analysis

### Debug Log Format
```
[1] INFO | kernel_main :: Starting KOS kernel
[2] DEBUG | memory_init :: Heap initialized at 0x300000
[3] INFO | interrupt_init :: IDT setup complete
[4] ERROR | page_fault :: Invalid page access at 0x12345678
[5] WARN | memory_alloc :: Heap nearly full
```

### Error Patterns
Common error patterns to watch for:
- Page faults and memory errors
- Invalid instruction exceptions
- Stack corruption indicators
- Resource allocation failures
- Interrupt handler errors

### Performance Analysis
Performance-related log entries:
```
[INFO] Timing [function_name]: 1234567 cycles
[INFO] Performance Alert: High CPU usage detected
[INFO] Memory Usage: 75% (1572864/2097152 bytes)
```

## Advanced Debugging

### Custom Breakpoints
```gdb
# Conditional breakpoints
(gdb) break kernel_main if argc > 1

# Hardware breakpoints
(gdb) hbreak *0x100000

# Watchpoints
(gdb) watch *0x12345678
```

### Memory Mapping
Understanding KOS memory layout:
```
0x100000 - 0x1FFFFF  # Kernel code (1MB)
0x200000 - 0x2FFFFF  # Kernel data (1MB)
0x300000 - 0x4FFFFF  # Heap area (2MB)
0x7FFFFF - 0x800000  # Stack area (4KB)
```

### Stack Analysis
```gdb
# Examine stack
(gdb) x/20xw $esp
(gdb) backtrace
(gdb) info frame

# Find return addresses
(gdb) x/wx $ebp
(gdb) info frame 1
```

## Troubleshooting

### GDB Connection Issues
```bash
# Check if QEMU is in debug mode
ps aux | grep qemu-system-x86_64

# Verify GDB is installed
which gdb-multiarch
which gdb

# Check port availability
netstat -an | grep 1234
```

### Monitor Connection Problems
```bash
# Test telnet connectivity
telnet localhost 1234

# Check QEMU monitor configuration
ps aux | grep qemu
```

### Symbol Loading Issues
```gdb
# Load symbols manually
(gdb) symbol-file dist/x86_64/kernel.bin

# Check symbol table
(gdb) info functions
(gdb) info variables
```

## Debug Scenarios

### Kernel Panic Debugging
1. **Reproduce**: Run in debug mode
2. **Break**: Set breakpoint at panic location
3. **Analyze**: Examine registers and stack
4. **Trace**: Use backtrace to find cause

### Memory Corruption Detection
1. **Pattern Fill**: Initialize memory with known patterns
2. **Validation**: Check patterns periodically
3. **Breakpoints**: Set on memory allocation
4. **Watchpoints**: Monitor critical memory locations

### Performance Issues
1. **Profiling**: Use timing measurements
2. **Hotspots**: Identify slow functions
3. **Optimization**: Focus on high-impact areas
4. **Validation**: Measure improvements

## Debug Configuration Files

### GDB Script Location
- File: `debug_log/gdb_script.txt`
- Auto-generated by debug tools
- Customizable for specific debugging needs

### QEMU Configuration
- Monitor port: 1234
- GDB port: 1234 (shared)
- Serial output: `debug_log/debug_log.txt`
- Trace output: `debug_log/qemu_trace.log`

## Best Practices

### Effective Debugging
1. **Start Simple**: Begin with basic breakpoints
2. **Isolate Issues**: Test components individually
3. **Use Logging**: Add debug prints for visibility
4. **Document**: Keep notes on debugging sessions
5. **Version Control**: Track debugging changes

### Common Pitfalls
- **Heisenbugs**: Issues that disappear when debugging
- **Timing Issues**: Problems that only occur at full speed
- **Memory Leaks**: Gradual memory consumption
- **Stack Overflows**: Corrupted return addresses

### Performance Considerations
- Debug builds have overhead
- Logging impacts performance
- Breakpoints slow execution
- Monitor output adds latency

## Integration

### with Build System
Debug builds automatically include:
- Debug symbols (`-g` flag)
- Additional logging
- Optimizations disabled
- Assertions enabled

### with Performance Monitor
Debug mode integrates with:
- Performance metrics collection
- Alert system
- Timing measurements
- Resource tracking

### with QEMU
Debug configuration includes:
- GDB server (`-s -S`)
- Tracing enabled (`-d int,cpu_reset`)
- Log output (`-D debug_log/qemu_trace.log`)
- Monitor access (`-monitor telnet:1234`)
