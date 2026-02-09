#pragma once

// =============================================================================
// KOS - Core Type Definitions
// =============================================================================

// Include standard headers first
#include <stdint.h>
#include <stddef.h>

// Standard types for freestanding environment
#ifndef __cplusplus
#include <stdbool.h>
#else
#include <stdbool.h>
#endif

// KOS-specific types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t  byte;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uintptr_t uptr;
typedef intptr_t  iptr;

typedef size_t    usize;
typedef ptrdiff_t  isize;

typedef bool      b8;

// Status codes
typedef enum {
    KOS_SUCCESS = 0,
    KOS_ERROR_INVALID_PARAM = -1,
    KOS_ERROR_OUT_OF_MEMORY = -2,
    KOS_ERROR_NOT_FOUND = -3,
    KOS_ERROR_PERMISSION_DENIED = -4,
    KOS_ERROR_IO_ERROR = -5,
    KOS_ERROR_TIMEOUT = -6,
    KOS_ERROR_NOT_IMPLEMENTED = -7,
    KOS_ERROR_INVALID_STATE = -8
} kos_result_t;

// Memory types
typedef enum {
    KOS_MEMORY_TYPE_KERNEL = 0,
    KOS_MEMORY_TYPE_USER = 1,
    KOS_MEMORY_TYPE_DEVICE = 2,
    KOS_MEMORY_TYPE_RESERVED = 3
} kos_memory_type_t;

// Interrupt levels
typedef enum {
    KOS_IRQ_LEVEL_DISABLED = 0,
    KOS_IRQ_LEVEL_LOW = 1,
    KOS_IRQ_LEVEL_NORMAL = 2,
    KOS_IRQ_LEVEL_HIGH = 3,
    KOS_IRQ_LEVEL_CRITICAL = 4
} kos_irq_level_t;

// Forward declarations
struct kos_cpu_state;
struct kos_process;
struct kos_thread;
