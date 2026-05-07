/* lib/kprint.c */

#include <lib/string/string.h>
#include <arch/general/platform.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void kprint(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		put_char(str[i]);
	}
}

void kprintln(const char* str)
{
	kprint(str);
	kprint("\n");
}

void kprintf(const char* str, ...)
{
    va_list args;
    va_start(args, str);

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char character = (uint8_t)str[i];

        switch (character)
        {
        case '%':
        {
            char type = (uint8_t)str[++i];
            switch (type)
            {
            case '%':
                put_char('%');
                break;
            case 'c':
            {
                char c = (char)(va_arg(args, int));
                put_char(c);
                break;
            }
            case 's':
            {
                char *s = va_arg(args, char *);
                if (s) {
                    // Write string directly to avoid recursion
                    for (char *p = s; *p; p++) {
                        put_char(*p);
                    }
                }
                break;
            }
            case 'd':
            {
                int d = va_arg(args, int);
		if (d == 0)
		{
			put_char('0');
			break;
		}
		char* num_str = itoa(d);
                for (char *p = num_str; *p; p++) {
                    put_char(*p);
                }
                break;
            }
            case 'x':
            {
                unsigned int num = va_arg(args, unsigned int);
                // Simple hex conversion
                char hex_str[20];
                char *ptr = hex_str + 19;
                *ptr = '\0';
                ptr--;

                if (num == 0) {
                    *ptr = '0';
                    put_char('0');
                } else {
                    while (num > 0) {
                        int digit = num % 16;
                        *ptr = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                        num /= 16;
                        ptr--;
                    }
                    ptr++;
                    while (*ptr) {
                        put_char(*ptr++);
                    }
                }
                break;
            }
            default:
                put_char('%');
                put_char(type);
                break;
            }
            break;
        }
        default:
            put_char(character);
            break;
        }
    }

    va_end(args);
}
