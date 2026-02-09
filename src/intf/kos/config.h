#pragma once

// =============================================================================
// KOS - Kernel Operating System Configuration
// =============================================================================

// Version Information
#define KOS_VERSION_MAJOR 0
#define KOS_VERSION_MINOR 1
#define KOS_VERSION_PATCH 0
#define KOS_VERSION_STRING "0.1.0"

// Architecture Configuration
#define KOS_ARCH_X86_64 1
#define KOS_POINTER_SIZE 64

// Memory Configuration
#define KOS_HEAP_START_ADDR    0x200000  // 2MB
#define KOS_HEAP_SIZE          0x200000  // 2MB
#define KOS_STACK_SIZE         0x10000   // 64KB
#define KOS_PAGE_SIZE          4096

// Hardware Configuration
#define KOS_SERIAL_PORT        0x3F8
#define KOS_VGA_MEMORY_ADDR    0xB8000
#define KOS_VGA_WIDTH          80
#define KOS_VGA_HEIGHT         25

// System Limits
#define KOS_MAX_IRQ_HANDLERS   16
#define KOS_MAX_KEYBOARD_LISTENERS 10
#define KOS_MAX_FILENAME_LENGTH 256

// Debug Configuration
#define KOS_DEBUG_ENABLED      1
#define KOS_LOG_LEVEL_DEBUG    0
#define KOS_LOG_LEVEL_INFO     1
#define KOS_LOG_LEVEL_WARN     2
#define KOS_LOG_LEVEL_ERROR    3
#define KOS_LOG_LEVEL_CRITICAL 4

// Feature Flags
#define KOS_FEATURE_VGA        1
#define KOS_FEATURE_KEYBOARD   1
#define KOS_FEATURE_TIMER      1
#define KOS_FEATURE_SERIAL     1
#define KOS_FEATURE_HEAP       1
