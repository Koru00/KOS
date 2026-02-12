#include "debug/debug.h"
#include "print.h"
#include <stdarg.h>

// This file contains simplified logging implementations
// The main implementations are in debug.c

// Forward declaration
static int vsnprintf(char* str, size_t size, const char* format, va_list args);

void log_init(void) {
    init_serial();
}

void log_printf(log_level_t level, const char* format, ...) {
    if (level < LOG_LEVEL_DEBUG || level > LOG_LEVEL_CRITICAL) {
        return;
    }
    
    // Print level prefix
    const char* level_strings[] = {
        "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"
    };
    
    serial_print("[");
    serial_print(level_strings[level]);
    serial_print("] ");
    
    // Print formatted message
    va_list args;
    va_start(args, format);
    
    // Use serial_print with va_args
    char buffer[512];
    // Simple va_args handling for now
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

void log_debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    serial_print("[DEBUG] ");
    
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    serial_print("[INFO] ");
    
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

void log_warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    serial_print("[WARN] ");
    
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    serial_print("[ERROR] ");
    
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

void log_critical(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    serial_print("[CRITICAL] ");
    
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    serial_print(buffer);
    serial_print("\n");
    
    va_end(args);
}

// Simple vsnprintf implementation
static int vsnprintf(char* str, size_t size, const char* format, va_list args) {
    if (!str || size == 0) return 0;
    
    size_t written = 0;
    
    for (size_t i = 0; format[i] != '\0' && written < size - 1; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == '\0') break;
            
            switch (format[i]) {
                case '%':
                    if (written < size - 1) str[written++] = '%';
                    break;
                case 'c':
                    if (written < size - 1) {
                        char c = (char)va_arg(args, int);
                        str[written++] = c;
                    }
                    break;
                case 's':
                {
                    char* s = va_arg(args, char*);
                    if (s) {
                        while (*s && written < size - 1) {
                            str[written++] = *s++;
                        }
                    }
                    break;
                }
                case 'd':
                {
                    int d = va_arg(args, int);
                    // Simple integer to string conversion
                    if (d == 0) {
                        if (written < size - 1) str[written++] = '0';
                    } else {
                        int temp = d;
                        if (temp < 0) {
                            if (written < size - 1) str[written++] = '-';
                            temp = -temp;
                        }
                        char digits[32];
                        int digit_count = 0;
                        while (temp > 0) {
                            digits[digit_count++] = '0' + (temp % 10);
                            temp /= 10;
                        }
                        for (int j = digit_count - 1; j >= 0 && written < size - 1; j--) {
                            str[written++] = digits[j];
                        }
                    }
                    break;
                }
                case 'x':
                {
                    unsigned int x = va_arg(args, unsigned int);
                    const char* hex_chars = "0123456789ABCDEF";
                    if (x == 0) {
                        if (written < size - 1) str[written++] = '0';
                    } else {
                        char digits[32];
                        int digit_count = 0;
                        while (x > 0) {
                            digits[digit_count++] = hex_chars[x % 16];
                            x /= 16;
                        }
                        for (int j = digit_count - 1; j >= 0 && written < size - 1; j--) {
                            str[written++] = digits[j];
                        }
                    }
                    break;
                }
                default:
                    if (written < size - 1) str[written++] = '%';
                    if (written < size - 1) str[written++] = format[i];
                    break;
            }
        } else {
            if (written < size - 1) str[written++] = format[i];
        }
    }
    
    str[written] = '\0';
    return written;
}
