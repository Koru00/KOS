# KOS v2 project structure

## linker:

contain linker files for different arch

## include:

contain global headers (arch independent)

### kernel:

    - arch.h: contain the definition for arch specific operation (like init_cpu)

### arch:

    arch-specific headers

    - platform.c: contain the implementation of the arch specific operation contained in include/kernel/arch.h

## kernel:

core kernel (arch independent logic)

### main.c: kernel entry point (after arch boot hands over)

## arch:

every thing platform specfic here

## lib:

kernel standard library

## drivers:

hardware drivers (some may be arch specific)

## filesystem (fs):

filesystem layer

## init:

system initialization sequence

## tools:

just some tools for help
