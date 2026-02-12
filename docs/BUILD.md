# KOS Build System

## Overview
The KOS build system uses Docker for consistent cross-compilation and provides multiple build configurations.

## Usage

### Basic Build
```bash
build.bat
```

### Build Options

#### 1. Standard Build
Default optimized build for normal development and testing.

#### 2. Debug Build
Includes debug symbols and additional logging for development.

#### 3. Clean Build
Removes all build artifacts and generated files.

#### 4. Build and Run
Builds the kernel and immediately launches it.

## Build Process

1. **Docker Environment**: Uses `myos-buildenv` container
2. **Cross-compilation**: Targets x86_64 architecture
3. **ISO Generation**: Creates bootable ISO with GRUB
4. **Output**: `dist/x86_64/kernel.iso`

## Dependencies

- Docker Desktop
- QEMU (for testing)
- x86_64-elf-gcc toolchain (in Docker)

## Build Artifacts

```
dist/x86_64/
├── kernel.bin    # Raw kernel binary
└── kernel.iso    # Bootable ISO image
```

## Troubleshooting

### Build Failures
- Check Docker is running
- Verify source files exist
- Check for syntax errors

### Missing ISO
- Ensure build completed successfully
- Check `dist/x86_64/` directory

### Docker Issues
```bash
# Restart Docker daemon
sudo systemctl restart docker

# Rebuild container
docker build -t myos-buildenv buildenv/
```

## Advanced Options

### Environment Variables
```bash
# Enable debug symbols
DEBUG=1

# Performance optimization
PERFORMANCE=1
```

### Make Targets
```bash
make all          # Build everything
make clean        # Clean artifacts
make run          # Build and run
make debug        # Debug build
```

## Build Configuration

The build system is configured in `Makefile`:
- Source directories: `src/impl/`
- Include directories: `src/intf/`
- Output directory: `dist/x86_64/`
- Linker script: `targets/x86_64/linker.ld`

## Performance

Build times typically:
- Clean build: ~2-3 minutes
- Incremental build: ~30 seconds
- Docker overhead: ~10 seconds
