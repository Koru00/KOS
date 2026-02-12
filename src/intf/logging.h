#pragma once

#include <stdint.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

// Logging functions
void log_init(void);
void log_printf(log_level_t level, const char* format, ...);
void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warn(const char* format, ...);
void log_error(const char* format, ...);
void log_critical(const char* format, ...);

// Convenience macros
#define LOG_DEBUG(...) log_debug(__VA_ARGS__)
#define LOG_INFO(...)  log_info(__VA_ARGS__)
#define LOG_WARN(...)  log_warn(__VA_ARGS__)
#define LOG_ERROR(...) log_error(__VA_ARGS__)
#define LOG_CRITICAL(...) log_critical(__VA_ARGS__)
