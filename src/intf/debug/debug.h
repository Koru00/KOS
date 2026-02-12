#pragma once

#include <stdint.h>
#include <stddef.h>

// Unified logging system
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

// Legacy compatibility
typedef enum {
    LOG_INFO = LOG_LEVEL_INFO,
    LOG_WARNING = LOG_LEVEL_WARN,
    LOG_ERROR = LOG_LEVEL_ERROR,
    LOG_DEBUG = LOG_LEVEL_DEBUG,
    LOG_FATAL = LOG_LEVEL_CRITICAL
} log_type_t;

// Core logging functions
void log_init(void);
void log_printf(log_level_t level, const char* format, ...);
void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warn(const char* format, ...);
void log_error(const char* format, ...);
void log_critical(const char* format, ...);

// Serial output functions
void serial_print(const char* str, ...);
void init_serial(void);

// Legacy compatibility functions
void log_message(const char* func, const char* message, const log_type_t type);
void serial_log(void);

// Convenience macros
#define LOG_DEBUG(...) log_debug(__VA_ARGS__)
#define LOG_INFO(...)  log_info(__VA_ARGS__)
#define LOG_WARN(...)  log_warn(__VA_ARGS__)
#define LOG_ERROR(...) log_error(__VA_ARGS__)
#define LOG_CRITICAL(...) log_critical(__VA_ARGS__)
