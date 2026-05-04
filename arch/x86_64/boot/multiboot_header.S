/* arch/x86_64/boot/multiboot_header.S */

.intel_syntax noprefix

.section .multiboot_header
header_start:
    # magic number (multiboot2)
    .long 0xe85250d6

    # architecture (protected mode i386)
    .long 0

    # header length
    .long header_end - header_start

    # checksum
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    # end tag
    .short 0
    .short 0
    .long 8

header_end:
