#include "strcmp.h"

int str_eq(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0; // not equal
        a++;
        b++;
    }
    return *a == *b; // both must be null terminators
}
