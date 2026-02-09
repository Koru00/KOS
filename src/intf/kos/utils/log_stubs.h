#pragma once

#include <stdarg.h>

// =============================================================================
// KOS - Logging Stubs Header
// =============================================================================

// Logging functions
void log_info(const char* format, ...);
void log_error(const char* format, ...);
void log_warn(const char* format, ...);
void log_debug(const char* format, ...);
void log_critical(const char* format, ...);
