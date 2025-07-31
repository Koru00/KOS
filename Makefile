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

# Flags
#CFLAGS        := -ffreestanding -Wall -Wextra -Wpedantic -Werror -I src/intf
CFLAGS        := -ffreestanding -Wall -Wextra -Wpedantic  -I src/intf
CFLAGS       += -MMD -MP             # dependency generation
ASFLAGS       := -f elf64
LDFLAGS       := -n -T $(LINKER_SCRIPT)

# =============================================================================
# Derived Lists
# =============================================================================

# Source files
C_SOURCES     := $(shell find $(SRC_DIR) -type f -name "*.c")
ASM_SOURCES   := $(shell find $(SRC_DIR) -type f -name "*.asm")

c_include_dirs := $(shell find src/intf -type d)
c_include_flags := $(foreach dir,$(c_include_dirs),-I$(dir))

# Object files
C_OBJECTS     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS   := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS       := $(C_OBJECTS) $(ASM_OBJECTS)

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
	$(CC) $(CFLAGS) -I  $(c_include_flags) -c $< -o $@

# =============================================================================
# Assemble ASM sources
# =============================================================================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	@echo "[AS] $<"
	$(AS) $(ASFLAGS) $< -o $@

$(ISO_ROOT)/boot/kernel.bin: $(DIST_DIR)/kernel.bin
	@mkdir -p $(ISO_ROOT)/boot
	@cp $(DIST_DIR)/kernel.bin $(ISO_ROOT)/boot/kernel.bin

$(DIST_DIR)/$(ISO_NAME): $(ISO_ROOT)/boot/kernel.bin
	@mkdir -p $(DIST_DIR)
	@chmod -R 777 $(DIST_DIR)
	@echo "[INFO] Generating ISO $(ISO_NAME)"
	$(GRUB) /usr/lib/grub/i386-pc -o $@ $(ISO_ROOT)

# =============================================================================
# Run in QEMU
# =============================================================================
.PHONY: run
run: all
	@echo "[INFO] Launching QEMU"
	$(QEMU) $(KVM_FLAG) -cpu host -smp $(QEMU_CORES) -m $(QEMU_RAM) \
		-cdrom $(DIST_DIR)/$(ISO_NAME) \
		-serial file:debug_log/.debug_log.txt \
		-monitor stdio -boot d

# =============================================================================
# Clean up artifacts
# =============================================================================
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

# =============================================================================
# Include dependency files
# =============================================================================
-include $(DEPS)
