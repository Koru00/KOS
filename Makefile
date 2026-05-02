# =========================
# CONFIG
# =========================

ARCH ?= x86_64

CC := gcc
LD := ld

BUILD_DIR := build/$(ARCH)
KERNEL := kernel.elf

# =========================
# FLAGS
# =========================

COMMON_FLAGS := -ffreestanding -nostdlib -Iinclude -Wall -Wextra

CFLAGS_64 := $(COMMON_FLAGS) -O2 -m64 -MMD -MP
CFLAGS_32 := $(COMMON_FLAGS) -O2 -m32 -MMD -MP

ASFLAGS_64 := $(COMMON_FLAGS) -m64
ASFLAGS_32 := $(COMMON_FLAGS) -m64

LDFLAGS := -nostdlib -T linker/$(ARCH).ld

ifeq ($(ARCH),x86_64)
	LDFLAGS += -m elf_x86_64
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

# C files are built as 64-bit kernel code
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "CC64 $<"
	$(CC) $(CFLAGS_64) -c $< -o $@

# Assembly files: files ending in 32.S are built as 32-bit, everything else as 64-bit
$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@case "$<" in \
		*32.S) echo "AS32 $<"; $(CC) $(ASFLAGS_32) -c $< -o $@ ;; \
		*)     echo "AS64 $<"; $(CC) $(ASFLAGS_64) -c $< -o $@ ;; \
	esac

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
	qemu-system-aarch64 -M virt -cpu cortex-a57 -kernel $(BUILD_DIR)/$(KERNEL)
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
	@echo "C sources: $(words $(C_SOURCES))"
	@echo "ASM sources: $(words $(ASM_SOURCES))"
	@echo "Objects: $(words $(OBJECTS))"

.PHONY: all clean run debug info
