#include "ports.h"
#include "string.h"
#include "debug.h"
#include <stdarg.h>
#include <stddef.h>

#define SERIAL_PORT 0x3F8

#define DEBUG_MODE 1

const char* LOG_TYPE_STR[] = {
	[LOG_INFO] = "INFO",
	[LOG_WARNING] = "WARNING",
	[LOG_ERROR] = "ERROR",
	[LOG_DEBUG] = "DEBUG",
	[LOG_FATAL] = "FATAL"
};

int log_id = 1;

// Helper function to convert integer to string
static char* int_to_str(int num) {
    static char buffer[32];
    int i = 0;
    int is_negative = 0;
    
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    if (is_negative) {
        buffer[i++] = '-';
    }
    
    buffer[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    
    return buffer;
}

// Helper function to convert unsigned int to hex string
static char* uint_to_hex(unsigned int num) {
    static char buffer[32];
    const char* hex_chars = "0123456789ABCDEF";
    int i = 0;
    
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }
    
    while (num > 0) {
        buffer[i++] = hex_chars[num % 16];
        num /= 16;
    }
    
    buffer[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    
    return buffer;
}

void init_serial(void) {
    // Disable interrupts
    port_byte_out(SERIAL_PORT + 1, 0x00);
    
    // Enable DLAB (set baud rate divisor)
    port_byte_out(SERIAL_PORT + 3, 0x80);
    
    // Set divisor to 3 (lo byte) for 38400 baud
    port_byte_out(SERIAL_PORT + 0, 0x03);
    port_byte_out(SERIAL_PORT + 1, 0x00);
    
    // 8 bits, no parity, one stop bit
    port_byte_out(SERIAL_PORT + 3, 0x03);
    
    // Enable FIFO, clear them, with 14-byte threshold
    port_byte_out(SERIAL_PORT + 2, 0xC7);
    
    // IRQs enabled, RTS/DSR set
    port_byte_out(SERIAL_PORT + 4, 0x0B);
    
    // Set in loopback mode, test the serial chip
    port_byte_out(SERIAL_PORT + 4, 0x1E);
    
    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    port_byte_out(SERIAL_PORT + 0, 0xAE);
    
    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    port_byte_out(SERIAL_PORT + 4, 0x0F);
}

void log_message(const char* func, const char* message, const log_type_t type)
{
	serial_print("[");
	serial_print(int_to_str(log_id++));
	serial_print("] ");
	serial_print(LOG_TYPE_STR[type]);
	serial_print(" | ");
	serial_print(func);
	serial_print(" :: ");
	serial_print(message);
	serial_print("\n");
}

void serial_write(char c) {
	#if DEBUG_MODE
    while ((port_byte_in(SERIAL_PORT + 5) & 0x20) == 0);
    port_byte_out(SERIAL_PORT, c);
	#endif
}

void serial_print(const char* str, ...) 
{
    va_list args;
    va_start(args, str);
    
    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char character = str[i];
        
        if (character == '%') {
            i++; // Move to format specifier
            if (str[i] == '\0') break;
            
            char type = str[i];
            switch (type)
            {
                case '%':
                    serial_write('%');
                    break;
                case 'c':
                {
                    char c = (char)(va_arg(args, int));
                    serial_write(c);
                    break;
                }
                case 's':
                {
                    char *s = va_arg(args, char *);
                    if (s) {
                        serial_print(s);
                    }
                    break;
                }
                case 'd':
                {
                    int d = va_arg(args, int);
                    serial_print(int_to_str(d));
                    break;
                }
                case 'x':
                case 'X':
                {
                    unsigned int num = va_arg(args, unsigned int);
                    serial_print(uint_to_hex(num));
                    break;
                }
                case 'u':
                {
                    unsigned int u = va_arg(args, unsigned int);
                    serial_print(int_to_str((int)u));
                    break;
                }
                default:
                    serial_write('%');
                    serial_write(type);
                    break;
            }
        } else {
            serial_write(character);
        }
    }
    
    va_end(args);
}

void serial_log() {
    // Placeholder for future logging functionality
}