#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "hal/hal_core.h"

// =============================================================================
// KOS - HAL Bridge Functions
// =============================================================================

// KOS memory manager (global)
struct {
    void* base;
    size_t size;
    size_t used;
    size_t free;
} g_memory_manager = {0};

// HAL memory functions
void* hal_malloc(size_t size) {
    // Simple stub implementation
    static uint8_t* heap_end = (uint8_t*)0x10000000; // 16MB mark
    void* ptr = heap_end;
    heap_end += size;
    return ptr;
}

void* hal_memalign(size_t alignment, size_t size) {
    // Simple stub implementation
    static uint8_t* heap_end = (uint8_t*)0x10000000; // 16MB mark
    uintptr_t addr = (uintptr_t)heap_end;
    addr = (addr + alignment - 1) & ~(alignment - 1);
    void* ptr = (void*)addr;
    heap_end = (uint8_t*)ptr + size;
    return ptr;
}

void hal_free(void* ptr) {
    // Simple stub - do nothing for now
    (void)ptr;
}

uint64_t hal_get_timestamp(void) {
    // Simple stub - return a counter
    static uint64_t counter = 0;
    return ++counter;
}

void* hal_memset(void* dest, int c, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    while (n--) {
        *d++ = (unsigned char)c;
    }
    return dest;
}

void* hal_memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

// HAL string functions
int hal_vsnprintf(char* str, size_t size, const char* format, va_list args) {
    // Simple stub implementation
    (void)format;
    (void)args;
    if (str && size > 0) {
        hal_strcpy(str, "[FORMAT STUB]");
    }
    return 0;
}

int hal_snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = hal_vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}

char* hal_strcpy(char* dest, const char* src) {
    if (!dest || !src) {
        return dest;
    }
    
    char* original_dest = dest;
    while ((*dest++ = *src++)) {
        // Copy characters
    }
    return original_dest;
}

char* hal_strncpy(char* dest, const char* src, size_t n) {
    if (!dest || !src) {
        return dest;
    }
    
    char* original_dest = dest;
    while (n-- && (*dest++ = *src++)) {
        // Copy characters
    }
    if (n == 0) {
        *dest = '\0';
    }
    return original_dest;
}

int hal_strcmp(const char* str1, const char* str2) {
    if (!str1 || !str2) {
        return str1 == str2 ? 0 : (str1 ? 1 : -1);
    }
    
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

size_t hal_strlen(const char* str) {
    if (!str) {
        return 0;
    }
    
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

// Legacy KOS functions for compatibility
void* kos_memset(void* dest, int c, size_t n) {
    return hal_memset(dest, c, n);
}

void* kos_memcpy(void* dest, const void* src, size_t n) {
    return hal_memcpy(dest, src, n);
}

int kos_strcmp(const char* str1, const char* str2) {
    return hal_strcmp(str1, str2);
}

char* kos_strcpy(char* dest, const char* src) {
    return hal_strcpy(dest, src);
}

// KOS memory management functions for HAL compatibility
void* kos_memory_alloc(size_t size) {
    return hal_malloc(size);
}

void kos_memory_free(void* ptr) {
    hal_free(ptr);
}

void kos_memory_init(void* base, size_t size) {
    // Simple stub - do nothing for now
    (void)base;
    (void)size;
}

void* hal_get_interface(void) {
    // Simple stub - return NULL for now
    return NULL;
}

// HAL system functions
hal_result_t hal_init(void) {
    // Simple stub - return success for now
    return HAL_SUCCESS;
}

hal_result_t hal_shutdown(void) {
    // Simple stub - return success for now
    return HAL_SUCCESS;
}

// HAL CPU functions
void hal_cpu_halt(void) {
    __asm__ volatile("hlt");
}

void hal_interrupt_disable(void) {
    __asm__ volatile("cli");
}

void hal_interrupt_enable(void) {
    __asm__ volatile("sti");
}

// Logging functions
void log_info(const char* format, ...) {
    (void)format;
    // TODO: Implement proper logging
}

void log_error(const char* format, ...) {
    (void)format;
    // TODO: Implement proper error logging
}

void log_warn(const char* format, ...) {
    (void)format;
    // TODO: Implement proper warning logging
}

void log_debug(const char* format, ...) {
    (void)format;
    // TODO: Implement proper debug logging
}

void log_critical(const char* format, ...) {
    (void)format;
    // TODO: Implement proper critical logging
}
