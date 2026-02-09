#pragma once

#include "../types.h"

// =============================================================================
// KOS - Port I/O Interface
// =============================================================================

// Port I/O functions
static inline u8 kos_port_byte_in(u16 port) {
    u8 result;
    asm volatile("in %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void kos_port_byte_out(u16 port, u8 data) {
    asm volatile("out %0, %1" : : "a"(data), "Nd"(port));
}

static inline u16 kos_port_word_in(u16 port) {
    u16 result;
    asm volatile("in %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void kos_port_word_out(u16 port, u16 data) {
    asm volatile("out %0, %1" : : "a"(data), "Nd"(port));
}

static inline u32 kos_port_dword_in(u16 port) {
    u32 result;
    asm volatile("in %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void kos_port_dword_out(u16 port, u32 data) {
    asm volatile("out %0, %1" : : "a"(data), "Nd"(port));
}

// Legacy compatibility macros
#define port_byte_in kos_port_byte_in
#define port_byte_out kos_port_byte_out
#define port_word_in kos_port_word_in
#define port_word_out kos_port_word_out
#define port_dword_in kos_port_dword_in
#define port_dword_out kos_port_dword_out
