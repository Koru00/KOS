#pragma once

#include <stddef.h>
#include <stdint.h>

// =============================================================================
// KOS - HAL Utility Functions Header
// =============================================================================

// String functions
size_t hal_strlen(const char* str);
char* hal_strcpy(char* dest, const char* src);
int hal_strcmp(const char* str1, const char* str2);
char* hal_strncpy(char* dest, const char* src, size_t n);
int hal_strncmp(const char* str1, const char* str2, size_t n);

// Memory functions
void* hal_memcpy(void* dest, const void* src, size_t n);
void* hal_memset(void* dest, int c, size_t n);
int hal_memcmp(const void* ptr1, const void* ptr2, size_t n);
void* hal_memmove(void* dest, const void* src, size_t n);
