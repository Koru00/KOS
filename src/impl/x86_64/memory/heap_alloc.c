#include <stdint.h>
#include <stddef.h>

#define HEAP_START 0x100000 // 1 Mb
#define HEAP_SIZE 0x10000 // 64 Kb

static uint8_t *heap = (uint8_t*)HEAP_START;
static size_t heap_used = 0;

void* malloc(size_t size)
{
    if (heap_used + size > HEAP_SIZE)
    {
        return NULL;
    }
    
    void* ptr = heap + heap_used;
    heap_used += size;
    return ptr;
}

// free(ptr)

void free_all()
{
    heap_used = 0;
}