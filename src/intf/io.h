#ifndef IO_H
#define IO_H

#include <stdint.h>

// DEPRECATED
/*
// Function to send a byte to an I/O port
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a" (value), "Nd" (port));
}

// Function to read a byte from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

#endif // IO_H*/
