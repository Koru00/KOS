#pragma once
#include <stdint.h>

#define IDT_ENTRIES 256



    struct IDTEntry {
    uint16_t offset_low;     // bits 0–15
    uint16_t selector;       // code segment selector in GDT
    uint8_t  ist;            // bits 0–2 = IST index, rest = 0
    uint8_t  type_attr;      // type and attributes
    uint16_t offset_mid;     // bits 16–31
    uint32_t offset_high;    // bits 32–63
    uint32_t zero;           // reserved
} __attribute__((packed));


struct interrupt_descriptor
{
    uint16_t address_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t address_mid;
    uint32_t address_high;
    uint32_t reserved;
} __attribute__((packed));



    struct IDTPointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern void init_idt();
