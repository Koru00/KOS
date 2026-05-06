#==========================
# CONFIG
# =========================

ARCH ?= x86_64

BUILD_DIR := build/$(ARCH)
KERNEL := kernel.elf

# =========================
# COMMON FLAGS
# =========================

COMMON_FLAGS := -ffreestanding -nostdlib -Iinclude -Wall -Wextra -MMD -MP

# =========================
# ARCH-SPECIFIC CONFIG
# =========================

ifeq ($(ARCH),x86_64)

	CC := gcc
	LD := ld

	CFLAGS := $(COMMON_FLAGS) -O2 -m64
	ASFLAGS := -m64
	LDFLAGS := -nostdlib -T linker/x86_64.ld -m elf_x86_64

# -------------------------

else ifeq ($(ARCH),riscv64)

	CC := riscv64-unknown-elf-gcc
	LD := riscv64-unknown-elf-ld

	CFLAGS := $(COMMON_FLAGS) -O2 -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany	
	DFLAGS := -DUART_OUTPUT 
	ASFLAGS := $(CFLAGS)
	LDFLAGS := -nostdlib -T linker/riscv64.ld

# -------------------------

else ifeq ($(ARCH),aarch64)

	CC := aarch64-none-elf-gcc
	LD := aarch64-none-elf-ld

	CFLAGS := $(COMMON_FLAGS) -O2 -march=armv8-a
	ASFLAGS := $(CFLAGS)
	LDFLAGS := -nostdlib -T linker/aarch64.ld

else
$(error Unsupported ARCH: $(ARCH))
endif

# =========================
# SOURCE DISCOVERY
# =========================

C_SOURCES  := $(shell find kernel lib arch/$(ARCH) -name "*.c")
ASM_SOURCES := $(shell find arch/$(ARCH) -name "*.S")

# =========================
# OBJECTS
# =========================

C_OBJECTS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)
DEPS := $(OBJECTS:.o=.d)

# =========================
# DEFAULT TARGET
# =========================

all: $(BUILD_DIR)/$(KERNEL)

# =========================
# LINKING
# =========================

$(BUILD_DIR)/$(KERNEL): $(OBJECTS)
	@echo "LD $(KERNEL)"
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

# =========================
# COMPILATION RULES
# =========================

# C
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC $(ARCH) $<"
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

# ASM (no more fake 32/64 split)
$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo "AS $(ARCH) $<"
	$(CC) $(ASFLAGS) -c $< -o $@

# =========================
# DEPENDENCIES
# =========================

-include $(DEPS)

# =========================
# CLEAN
# =========================

clean:
	rm -rf build

# =========================
# RUN
# =========================

run:
ifeq ($(ARCH),x86_64)
	qemu-system-x86_64 -cdrom build/os.iso

else ifeq ($(ARCH),aarch64)
	qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -kernel $(BUILD_DIR)/$(KERNEL)

else ifeq ($(ARCH),riscv64)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic   -serial mon:stdio -bios none -kernel $(BUILD_DIR)/$(KERNEL) -d in_asm,cpu -D trace.log
endif

# =========================
# DEBUG
# =========================

debug:
ifeq ($(ARCH),x86_64)
	qemu-system-x86_64 -kernel $(BUILD_DIR)/$(KERNEL) -s -S
endif

# =========================
# INFO
# =========================

info:
	@echo "ARCH: $(ARCH)"
	@echo "CC: $(CC)"
	@echo "LD: $(LD)"
	@echo "C sources: $(words $(C_SOURCES))"
	@echo "ASM sources: $(words $(ASM_SOURCES))"
	@echo "Objects: $(words $(OBJECTS))"

.PHONY: all clean run debug info
