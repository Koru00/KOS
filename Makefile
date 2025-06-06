kernel_source_files := $(shell find src/impl/kernel -name '*.c')
kernel_object_files := $(foreach f,$(kernel_source_files),$(patsubst src/impl/kernel/%,build/kernel/%,$(f:.c=.o)))

x86_64_c_source_files := $(shell find src/impl/x86_64 -name '*.c')
x86_64_c_object_files := $(foreach f,$(x86_64_c_source_files),$(patsubst src/impl/x86_64/%,build/x86_64/%,$(f:.c=.o)))

c_include_dirs := $(shell find src/intf -type d)
c_include_flags := $(foreach dir,$(c_include_dirs),-I$(dir))

x86_64_asm_source_files := $(shell find src/impl/x86_64 -name '*.asm')
x86_64_asm_object_files := $(foreach f,$(x86_64_asm_source_files),$(patsubst src/impl/x86_64/%,build/x86_64/%,$(f:.asm=.o)))

x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files)

$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/intf $(c_include_flags) -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@ 

$(x86_64_c_object_files): build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/intf $(c_include_flags) -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files)
	mkdir -p dist/x86_64 && \
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files)
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso
