#include "string.h"
#include <stdint.h>
#include <stddef.h>

// =============================================================================
// KOS - String and Memory Utilities Implementation
// =============================================================================

// Memory operations
void* kos_memset(void* ptr, int value, size_t num) {
    if (!ptr) return NULL;
    
    uint8_t* p = (uint8_t*)ptr;
    while (num--) {
        *p++ = (uint8_t)value;
    }
    return ptr;
}

void* kos_memcpy(void* dest, const void* src, size_t num) {
    if (!dest || !src) return NULL;
    if (dest == src) return dest;
    
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    // Handle overlapping memory
    if (d < s) {
        // Copy forward
        while (num--) {
            *d++ = *s++;
        }
    } else {
        // Copy backward
        d += num;
        s += num;
        while (num--) {
            *--d = *--s;
        }
    }
    return dest;
}

int kos_memcmp(const void* ptr1, const void* ptr2, size_t num) {
    if (!ptr1 || !ptr2) return 0;
    
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    
    while (num--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

// String operations
size_t kos_strlen(const char* str) {
    if (!str) return 0;
    
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

char* kos_strcpy(char* dest, const char* src) {
    if (!dest || !src) return NULL;
    
    char* d = dest;
    while ((*d++ = *src++)) {
        // Copy until null terminator
    }
    return dest;
}

char* kos_strncpy(char* dest, const char* src, size_t n) {
    if (!dest || !src) return NULL;
    
    char* d = dest;
    size_t i;
    
    for (i = 0; i < n && src[i]; i++) {
        d[i] = src[i];
    }
    
    // Pad with null characters
    for (; i < n; i++) {
        d[i] = '\0';
    }
    
    return dest;
}

int kos_strcmp(const char* str1, const char* str2) {
    if (!str1 || !str2) return 0;
    
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int kos_strncmp(const char* str1, const char* str2, size_t n) {
    if (!str1 || !str2) return 0;
    
    while (n-- && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    
    if (n == SIZE_MAX) {
        return 0; // Strings are equal for n characters
    }
    
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

char* kos_strchr(const char* str, int c) {
    if (!str) return NULL;
    
    while (*str) {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    return NULL;
}

char* kos_strrchr(const char* str, int c) {
    if (!str) return NULL;
    
    char* last = NULL;
    while (*str) {
        if (*str == (char)c) {
            last = (char*)str;
        }
        str++;
    }
    return last;
}

// Utility functions
int kos_isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int kos_isxdigit(int c) {
    return ((c >= '0' && c <= '9') || 
            (c >= 'a' && c <= 'f') || 
            (c >= 'A' && c <= 'F'));
}

int kos_islower(int c) {
    return (c >= 'a' && c <= 'z');
}

int kos_isupper(int c) {
    return (c >= 'A' && c <= 'Z');
}

int kos_isalpha(int c) {
    return (kos_islower(c) || kos_isupper(c));
}

int kos_isalnum(int c) {
    return (kos_isalpha(c) || kos_isdigit(c));
}

int kos_toupper(int c) {
    return (kos_islower(c) ? (c - 'a' + 'A') : c);
}

int kos_tolower(int c) {
    return (kos_isupper(c) ? (c - 'A' + 'a') : c);
}

// Number to string conversion
char* kos_itoa(int value, char* str, int base) {
    if (!str || base < 2 || base > 36) return NULL;
    
    char* result = str;
    int sign = 0;
    
    // Handle negative numbers
    if (value < 0 && base == 10) {
        sign = 1;
        value = -value;
    }
    
    // Handle zero
    if (value == 0) {
        *result++ = '0';
        *result = '\0';
        return str;
    }
    
    // Convert to string
    char temp[32];
    int i = 0;
    
    while (value > 0) {
        int digit = value % base;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value /= base;
    }
    
    // Add sign if needed
    if (sign) {
        *result++ = '-';
    }
    
    // Reverse the string
    while (i > 0) {
        *result++ = temp[--i];
    }
    
    *result = '\0';
    return str;
}

char* kos_uitoa(unsigned int value, char* str, int base) {
    if (!str || base < 2 || base > 36) return NULL;
    
    char* result = str;
    
    // Handle zero
    if (value == 0) {
        *result++ = '0';
        *result = '\0';
        return str;
    }
    
    // Convert to string
    char temp[32];
    int i = 0;
    
    while (value > 0) {
        unsigned int digit = value % base;
        temp[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value /= base;
    }
    
    // Reverse the string
    while (i > 0) {
        *result++ = temp[--i];
    }
    
    *result = '\0';
    return str;
}

// String to number conversion
int kos_atoi(const char* str) {
    if (!str) return 0;
    
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert digits
    while (kos_isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

unsigned int kos_atou(const char* str) {
    if (!str) return 0;
    
    unsigned int result = 0;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    // Convert digits
    while (kos_isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result;
}

// Safe string operations
size_t kos_strnlen(const char* str, size_t maxlen) {
    if (!str) return 0;
    
    size_t len = 0;
    while (len < maxlen && str[len]) {
        len++;
    }
    return len;
}

int kos_strncmp_safe(const char* str1, const char* str2, size_t n) {
    if (!str1 || !str2) return 0;
    
    return kos_strncmp(str1, str2, n);
}

char* kos_strcpy_safe(char* dest, const char* src, size_t dest_size) {
    if (!dest || !src || dest_size == 0) return NULL;
    
    size_t src_len = kos_strlen(src);
    size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
    
    kos_memcpy(dest, src, copy_len);
    dest[copy_len] = '\0';
    
    return dest;
}

char* kos_strcat(char* dest, const char* src) {
    if (!dest || !src) return dest;
    
    char* ptr = dest;
    while (*ptr) {
        ptr++;
    }
    
    while (*src) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    
    return dest;
}
