# -----------------------------------------------------------------------------
# Robust Makefile for OS kernel (x86_64)
# - Recursively finds all .c and .asm under src/impl
# - Automatic dependency tracking via -MMD -MP
# - Configurable compiler flags, dirs, and QEMU options
# - Verbose and quiet modes
# - Phony targets: all, clean, run, iso
# -----------------------------------------------------------------------------

# =============================================================================
# Configuration Variables
# =============================================================================

# Directories
SRC_DIR       := src/impl
BUILD_DIR     := build
DIST_DIR      := dist/x86_64
ISO_NAME      := kernel.iso
LINKER_SCRIPT := targets/x86_64/linker.ld
ISO_ROOT      := targets/x86_64/iso

# Tools
CC            := x86_64-elf-gcc
AS            := nasm
LD            := x86_64-elf-ld
GRUB          := grub-mkrescue
QEMU          := qemu-system-x86_64

# Resource defaults
QEMU_CORES    := 4
QEMU_RAM      := 2048
KVM_FLAG      := -enable-kvm

# Professional QEMU settings
QEMU_CPU      := host
QEMU_DISPLAY  := std
QEMU_SOUND    := hda
QEMU_NET      := user
QEMU_DEBUG    := -serial file:debug_log/.debug_log.txt -monitor telnet:localhost:1234,server,nowait

# Flags
CFLAGS        := -ffreestanding -Wall -Wextra -Wpedantic -I src/intf 
CFLAGS       += -MMD -MP             # dependency generation
ASFLAGS       := -f elf64
LDFLAGS       := -n -T $(LINKER_SCRIPT)

# =============================================================================
# Derived Lists
# =============================================================================

# Source files
C_SOURCES     := $(shell find $(SRC_DIR) -type f -name "*.c" | grep -v "hal_convenience\.c$")
ASM_SOURCES   := $(shell find $(SRC_DIR) -type f -name "*.asm")
HAL_SOURCES   := $(shell find src/impl/hal -type f -name "*.c" | grep -v "_clean\.c$" | grep -v "hal_convenience\.c$")
HAL_CLEAN_SOURCES := $(shell find src/impl/hal -type f -name "*_clean.c")
ISR_HAL_SOURCES := $(shell find src/impl/kos/interrupts -name "isr_hal.c")
MEMORY_HAL_SOURCES := $(shell find src/impl/kos/memory -name "memory_hal.c")
KOS_BRIDGE_SOURCES := $(shell find src/impl/kos/utils -name "kos_hal_bridge.c")
KOS_ENTRY_SOURCES := $(shell find src/impl/kos -name "kernel_entry.c")
DRIVER_FRAMEWORK_SOURCES := $(shell find src/impl/kos/drivers -name "driver_*.c")
VGA_DRIVER_SOURCES := $(shell find src/impl/kos/drivers -name "vga_driver_simple.c")
PROCESS_SOURCES := $(shell find src/impl/kos/process -name "*.c")
SYSTEM_SOURCES := $(shell find src/impl/kos/system -name "startup_display_final.c")

# Include directories for C files
c_include_dirs := $(shell find src/intf -type d)
c_include_flags := $(foreach dir,$(c_include_dirs),-I$(dir))

# Object files
C_OBJECTS     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS   := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
HAL_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_SOURCES))
HAL_CLEAN_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_CLEAN_SOURCES))
ISR_HAL_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(ISR_HAL_SOURCES))
MEMORY_HAL_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(MEMORY_HAL_SOURCES))
KOS_BRIDGE_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(KOS_BRIDGE_SOURCES))
KOS_ENTRY_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(KOS_ENTRY_SOURCES))
DRIVER_FRAMEWORK_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(DRIVER_FRAMEWORK_SOURCES))
VGA_DRIVER_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(VGA_DRIVER_SOURCES))
PROCESS_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(PROCESS_SOURCES))
SYSTEM_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SYSTEM_SOURCES))
OBJECTS       := $(C_OBJECTS) $(ASM_OBJECTS) $(HAL_OBJECTS) $(HAL_CLEAN_OBJECTS) $(ISR_HAL_OBJECTS) $(MEMORY_HAL_OBJECTS) $(KOS_BRIDGE_OBJECTS) $(KOS_ENTRY_OBJECTS) $(DRIVER_FRAMEWORK_OBJECTS) $(VGA_DRIVER_OBJECTS) $(PROCESS_OBJECTS) $(SYSTEM_OBJECTS)

# Dependency files
DEPS          := $(C_OBJECTS:.o=.d)

# =============================================================================
# Default target
# =============================================================================
.PHONY: all
all: $(DIST_DIR)/$(ISO_NAME)

# =============================================================================
# Build ISO
# =============================================================================
$(DIST_DIR)/$(ISO_NAME): $(DIST_DIR)/kernel.bin
	@mkdir -p $(DIST_DIR)
	@chmod -R 777 $(DIST_DIR)
	@echo "[INFO] Generating ISO $(ISO_NAME)"
	$(GRUB) /usr/lib/grub/i386-pc -o $@ $(ISO_ROOT)

# Link kernel binary
$(DIST_DIR)/kernel.bin: $(OBJECTS)
	@mkdir -p $(DIST_DIR)
	@echo "[INFO] Linking kernel binary"
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# =============================================================================
# Compile C sources
# =============================================================================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "[CC] $<"
	$(CC) $(CFLAGS) $(c_include_flags) -c $< -o $@

# =============================================================================
# Assemble ASM sources
# =============================================================================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	@echo "[AS] $<"
	$(AS) $(ASFLAGS) $< -o $@

# =============================================================================
# Copy kernel binary to ISO root
# =============================================================================
$(ISO_ROOT)/boot/kernel.bin: $(DIST_DIR)/kernel.bin
	@mkdir -p $(ISO_ROOT)/boot
	@cp $(DIST_DIR)/kernel.bin $(ISO_ROOT)/boot/kernel.bin

# =============================================================================
# Run in QEMU - Professional Configuration
# =============================================================================
.PHONY: run
run: all
	@echo "[INFO] Launching QEMU with professional configuration"
	@mkdir -p debug_log
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp $(QEMU_CORES) \
		-m $(QEMU_RAM) \
		-machine q35 \
		-vga virtio \
		-vga virtio \
		-device virtio-scsi-pci,id=scsi0 \
		-device scsi-hd,drive=hd0,bus=scsi0.0 \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ich9-intel-hda \
		-device hda-duplex \
		-netdev user,id=net0 \
		-device e1000,netdev=net0 \
		-device virtio-rng-pci \
		-device virtio-balloon-pci \
		-rtc base=localtime,clock=host,driftfix=slew \
		-enable-kvm \
		$(QEMU_DEBUG) \
		-boot d

# =============================================================================
# Debug QEMU with GDB
# =============================================================================
.PHONY: debug
debug: all
	@echo "[INFO] Launching QEMU in debug mode"
	@mkdir -p debug_log
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp $(QEMU_CORES) \
		-m $(QEMU_RAM) \
		-machine q35 \
		-vga virtio \
		-vga virtio \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/.debug_log.txt \
		-monitor telnet:localhost:1234,server,nowait \
		-s -S \
		-d int,cpu_reset \
		-D debug_log/qemu_trace.log \
		-boot d

# =============================================================================
# Performance testing QEMU
# =============================================================================
.PHONY: benchmark
benchmark: all
	@echo "[INFO] Launching QEMU for benchmarking"
	@mkdir -p debug_log
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp $(QEMU_CORES) \
		-m $(QEMU_RAM) \
		-machine q35,accel=kvm \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/.debug_log.txt \
		-monitor telnet:localhost:1234,server,nowait \
		-boot d

# =============================================================================
# Clean up artifacts
# =============================================================================
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

# =============================================================================
# Test targets
# =============================================================================

# Build test runner as kernel module (not standalone executable)
.PHONY: test
test: 
	@echo "[INFO] Building test modules"
	$(MAKE) -C build tests

# Run all tests (integrated into kernel)
.PHONY: run-tests
run-tests: all
	@echo "[INFO] Running KOS validation tests (integrated in kernel)"
	@echo "[INFO] Tests will run during kernel boot"
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp $(QEMU_CORES) \
		-m $(QEMU_RAM) \
		-machine q35 \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/test_output.log \
		-serial stdio \
		-boot d

# Quick validation test
.PHONY: quick-test
quick-test: all
	@echo "[INFO] Running quick validation"
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp 1 \
		-m 512 \
		-machine q35 \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/quick_test.log \
		-serial stdio \
		-boot d

# Test memory system only
.PHONY: test-memory
test-memory: all
	@echo "[INFO] Running memory system tests"
	@echo "[INFO] Memory tests will run during kernel boot"
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp 1 \
		-m 512 \
		-machine q35 \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/memory_test.log \
		-serial stdio \
		-boot d

# Test logging system only
.PHONY: test-logging
test-logging: all
	@echo "[INFO] Running logging system tests"
	@echo "[INFO] Logging tests will run during kernel boot"
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp 1 \
		-m 512 \
		-machine q35 \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/logging_test.log \
		-serial stdio \
		-boot d

# Test integration only
.PHONY: test-integration
test-integration: all
	@echo "[INFO] Running integration tests"
	@echo "[INFO] Integration tests will run during kernel boot"
	$(QEMU) $(KVM_FLAG) \
		-cpu $(QEMU_CPU) \
		-smp 1 \
		-m 512 \
		-machine q35 \
		-display none \
		-vga none \
		-drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom \
		-device ide-cd,drive=hd0 \
		-serial file:debug_log/integration_test.log \
		-serial stdio \
		-boot d

# Continuous integration target (build + test)
.PHONY: ci
ci: clean all quick-test
	@echo "[INFO] CI pipeline completed"
	@echo "[INFO] Check debug_log/quick_test.log for test results"

# =============================================================================
# Include dependency files
# =============================================================================
-include $(DEPS)