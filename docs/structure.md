# Directory Structure
.
├── build
│   ├── kernel
│   │   └── main.o
│   └── x86_64
│       ├── boot
│       │   ├── header.o
│       │   ├── idt.o
│       │   ├── main64.o
│       │   ├── main.o
│       │   └── pic.o
│       ├── debug
│       │   └── debug.o
│       ├── drivers
│       │   ├── keyboard.o
│       │   ├── ports.o
│       │   └── video
│       │       ├── cursor.o
│       │       └── vga.o
│       ├── file
│       │   └── file.o
│       ├── interrupts
│       │   ├── idt.o
│       │   ├── irq.o
│       │   ├── irq_stubs.o
│       │   ├── isr.o
│       │   ├── pic.o
│       │   ├── pit.o
│       │   └── start.o
│       ├── library
│       │   └── io
│       │       └── print.o
│       ├── memory
│       │   └── heap_alloc.o
│       ├── power
│       │   └── power.o
│       ├── shell
│       │   └── commands.o
│       ├── string
│       │   ├── strcmp.o
│       │   └── string.o
│       └── utils
│           └── memset.o
├── build.bat
├── buildenv
│   └── Dockerfile
├── build.sh
├── debug_log
├── dist
│   └── x86_64
│       ├── kernel.bin
│       └── kernel.iso
├── docs
│   └── DEVMAP.md
├── logo.png
├── Makefile
├── README.md
├── run.bat
├── run.sh
├── src
│   ├── impl
│   │   ├── kernel
│   │   │   └── main.c
│   │   └── x86_64
│   │       ├── boot
│   │       │   ├── header.asm
│   │       │   ├── idt.asm
│   │       │   ├── main64.asm
│   │       │   ├── main.asm
│   │       │   └── pic.asm
│   │       ├── debug
│   │       │   └── debug.c
│   │       ├── drivers
│   │       │   ├── keyboard.c
│   │       │   ├── ports.c
│   │       │   └── video
│   │       │       ├── cursor.c
│   │       │       └── vga.c
│   │       ├── file
│   │       │   └── file.c
│   │       ├── interrupts
│   │       │   ├── idt.c
│   │       │   ├── irq.c
│   │       │   ├── irq_stubs.asm
│   │       │   ├── isr.c
│   │       │   ├── pic.c
│   │       │   ├── pit.c
│   │       │   └── start.asm
│   │       ├── library
│   │       │   └── io
│   │       │       └── print.c
│   │       ├── memory
│   │       │   └── heap_alloc.c
│   │       ├── power
│   │       │   └── power.c
│   │       ├── shell
│   │       │   └── commands.c
│   │       ├── string
│   │       │   ├── strcmp.c
│   │       │   └── string.c
│   │       └── utils
│   │           └── memset.c
│   └── intf
│       ├── commands.h
│       ├── cursor.h
│       ├── debug.h
│       ├── file.h
│       ├── heap_alloc.h
│       ├── idt.h
│       ├── io.h
│       ├── irq.h
│       ├── isr.h
│       ├── keyboard.h
│       ├── memset.h
│       ├── pic.h
│       ├── pit.h
│       ├── ports.h
│       ├── power.h
│       ├── print.h
│       ├── stdarg.h
│       ├── strcmp.h
│       ├── string.h
│       └── vga.h
└── targets
    └── x86_64
        ├── iso
        │   └── boot
        │       ├── grub
        │       │   └── grub.cfg
        │       └── kernel.bin
        └── linker.ld
