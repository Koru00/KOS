#pragma once

#include "../types.h"
#include "../config.h"
#include <stddef.h>
#include <stdarg.h>

// =============================================================================
// KOS - Logging System Interface
// =============================================================================

// Log levels
typedef enum {
    KOS_LOG_DEBUG = KOS_LOG_LEVEL_DEBUG,
    KOS_LOG_INFO = KOS_LOG_LEVEL_INFO,
    KOS_LOG_WARN = KOS_LOG_LEVEL_WARN,
    KOS_LOG_ERROR = KOS_LOG_LEVEL_ERROR,
    KOS_LOG_CRITICAL = KOS_LOG_LEVEL_CRITICAL
} kos_log_level_t;

// Log entry structure
typedef struct kos_log_entry {
    kos_log_level_t level;
    u64 timestamp;
    const char* function;
    const char* message;
    u32 thread_id;
} kos_log_entry_t;

// Log output interface
typedef struct kos_log_output {
    const char* name;
    kos_result_t (*write)(struct kos_log_output* output, const kos_log_entry_t* entry);
    kos_result_t (*flush)(struct kos_log_output* output);
    void* private_data;
} kos_log_output_t;

// Logger configuration
typedef struct kos_logger_config {
    kos_log_level_t min_level;
    kos_log_output_t* outputs[8];  // Support up to 8 outputs
    usize output_count;
    b8 enable_timestamps;
    b8 enable_function_names;
} kos_logger_config_t;

// Logger instance
typedef struct kos_logger {
    kos_logger_config_t config;
    u64 start_timestamp;
    u32 entry_count;
} kos_logger_t;

// Logging functions
kos_result_t kos_logger_init(kos_logger_t* logger, const kos_logger_config_t* config);
kos_result_t kos_logger_shutdown(kos_logger_t* logger);
kos_result_t kos_logger_add_output(kos_logger_t* logger, kos_log_output_t* output);

// Core logging functions
void kos_log_write(kos_logger_t* logger, kos_log_level_t level, const char* function, const char* message);
void kos_log_writef(kos_logger_t* logger, kos_log_level_t level, const char* function, const char* format, ...);

// Convenience macros
#define KOS_LOG_DEBUG(msg, ...) kos_log_writef(&g_logger, KOS_LOG_DEBUG, __func__, msg, ##__VA_ARGS__)
#define KOS_LOG_INFO(msg, ...)  kos_log_writef(&g_logger, KOS_LOG_INFO, __func__, msg, ##__VA_ARGS__)
#define KOS_LOG_WARN(msg, ...)  kos_log_writef(&g_logger, KOS_LOG_WARN, __func__, msg, ##__VA_ARGS__)
#define KOS_LOG_ERROR(msg, ...) kos_log_writef(&g_logger, KOS_LOG_ERROR, __func__, msg, ##__VA_ARGS__)
#define KOS_LOG_CRITICAL(msg, ...) kos_log_writef(&g_logger, KOS_LOG_CRITICAL, __func__, msg, ##__VA_ARGS__)

// Built-in log outputs
extern kos_log_output_t g_serial_log_output;
extern kos_log_output_t g_vga_log_output;

// Global logger instance
extern kos_logger_t g_logger;

// Output creation functions
kos_result_t kos_serial_log_output_create(kos_log_output_t* output);
kos_result_t kos_vga_log_output_create(kos_log_output_t* output);
