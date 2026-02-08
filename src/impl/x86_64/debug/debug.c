#include "ports.h"
#include "string.h"
#include "debug.h"
#include "stdarg.h"

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

void log_message(const char* func, const char* message, const log_type_t type)
{
	serial_print("[%d] %s | %s :: %s\n", log_id++, LOG_TYPE_STR[type] , func,message);
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

    for (size_t i = 0; 1; i++)
    {

        char character = (uint8_t)str[i];

        char sp[3];

        switch (character)
        {
        case '\0':
            goto end;
            break;
        case '%':
            char type = (uint8_t)str[++i];
            switch (type)
            {
            case '%':
                serial_write('%');
                break;
            case 'c':
                char c = (char)(va_arg(args, int));
                serial_write(c);
                break;
            case 's':
                char *s = va_arg(args, char *);
                serial_print(s);
                break;
            case 'd':
                int d = va_arg(args, int);
                serial_print(int_to_str(d));
                break;
            case 'x':
                unsigned int num = va_arg(args, unsigned int);
                serial_print(hex_to_str(num));
            }
            break;
        default:
            serial_write(character);
            break;
        }
    }

end:
    va_end(args);
}