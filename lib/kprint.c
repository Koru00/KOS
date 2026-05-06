#include "lib/kprint.h"
#include "lib/string/string.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void kprint(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		arch_putchar(str[i]);
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
        /*case '\0':
            goto end;
            break;
        case '\n':
            arch_putchar('\n');
            break;
        case '\t':
            arch_putchar('\t');
            break;*/
        case '%':
        {
            char type = (uint8_t)str[++i];
            switch (type)
            {
            case '%':
                arch_putchar('%');
                break;
            case 'c':
            {
                char c = (char)(va_arg(args, int));
                arch_putchar(c);
                break;
            }
            case 's':
            {
                char *s = va_arg(args, char *);
                if (s) {
                    // Write string directly to avoid recursion
                    for (char *p = s; *p; p++) {
                        arch_putchar(*p);
                    }
                }
                break;
            }
            case 'd':
            {
                int d = va_arg(args, int);
               	char* num_str = itoa(d);
                for (char *p = num_str; *p; p++) {
                    arch_putchar(*p);
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
                    arch_putchar('0');
                } else {
                    while (num > 0) {
                        int digit = num % 16;
                        *ptr = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                        num /= 16;
                        ptr--;
                    }
                    ptr++;
                    while (*ptr) {
                        arch_putchar(*ptr++);
                    }
                }
                break;
            }
            default:
                arch_putchar('%');
                arch_putchar(type);
                break;
            }
            break;
        }
        default:
            arch_putchar(character);
            break;
        }
    }

/*end:
    va_end(args);*/
}
