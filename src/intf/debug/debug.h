#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_FATAL
} log_type_t;

void log_message(const char* func, const char* message, const log_type_t type);
void serial_print(const char* str, ...);
void serial_log();
