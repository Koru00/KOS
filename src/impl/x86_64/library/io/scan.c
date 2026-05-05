#include "stdarg.h"
#include "vga.h"
#include "heap_alloc.h"

void scanf(char *output)
{
    int length = vga_line_l();
   output = realloc(output, length);
    for (int i = 0; i < length; i++)
    {
        output[i] = vga_read(i, 0);
    }
  output[length] = '\0'; // null-terminate
}
