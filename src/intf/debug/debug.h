#pragma once

typedef enum {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
	LOG_FATAL
} log_type;

void log_message(char* func, const char* message);
void serial_print(const char* str);
void serial_log();
