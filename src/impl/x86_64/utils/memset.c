#include <stdint.h>
#include <stdint.h>
#include <stddef.h>

void* memset(void* ptr, int value, size_t num) {
    unsigned char* p = ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}
