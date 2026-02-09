#pragma once

#include <stdint.h>
#include <stddef.h>

// =============================================================================
// KOS - String and Memory Utilities
// =============================================================================

// Memory operations
void* kos_memset(void* ptr, int value, size_t num);
void* kos_memcpy(void* dest, const void* src, size_t num);
int kos_memcmp(const void* ptr1, const void* ptr2, size_t num);

// String operations
size_t kos_strlen(const char* str);
char* kos_strcpy(char* dest, const char* src);
char* kos_strncpy(char* dest, const char* src, size_t n);
int kos_strcmp(const char* str1, const char* str2);
int kos_strncmp(const char* str1, const char* str2, size_t n);
char* kos_strchr(const char* str, int c);
char* kos_strrchr(const char* str, int c);

// Utility functions
int kos_isdigit(int c);
int kos_isxdigit(int c);
int kos_islower(int c);
int kos_isupper(int c);
int kos_isalpha(int c);
int kos_isalnum(int c);
int kos_toupper(int c);
int kos_tolower(int c);

// Number to string conversion
char* kos_itoa(int value, char* str, int base);
char* kos_uitoa(unsigned int value, char* str, int base);
char* kos_ltoa(long value, char* str, int base);
char*kos_ultoa(unsigned long value, char* str, int base);

// String to number conversion
int kos_atoi(const char* str);
long kos_atol(const char* str);
unsigned int kos_atou(const char* str);

// Safe string operations
size_t kos_strnlen(const char* str, size_t maxlen);
int kos_strncmp_safe(const char* str1, const char* str2, size_t n);
char* kos_strcpy_safe(char* dest, const char* src, size_t dest_size);
char* kos_strcat(char* dest, const char* src);
