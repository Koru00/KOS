/* arch/x86_64/boot/multiboot_header.S */

.section .multiboot_header
.align 8

header_start:
    .long 0xe85250d6                /* magic */
    .long 0                         /* architecture */
    .long header_end - header_start /* total length */
    /* checksum */
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    /* Add optional tags here if needed */

    /* end tag */
    .align 8
    .short 0
    .short 0
    .long 8
header_end:
