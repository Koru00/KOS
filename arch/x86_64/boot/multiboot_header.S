/* arch/x86_64/boot/multiboot_header.S */

.section .multiboot
.align 8

.long 0xe85250d6              /* magic */
.long 0                      /* architecture (i386) */
.long header_end - header_start
.long -(0xe85250d6 + 0 + (header_end - header_start))

header_start:

/* end tag */
.align 8
.short 0
.short 0
.long 8

header_end:
