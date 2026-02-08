#include <stdint.h>
#include <stddef.h>

#define HEAP_START 0x100000 // 1 Mb
#define HEAP_SIZE 0x10000 // 64 Kb

typedef struct block_header {
  size_t size;
  struct block_header* next;
  int free;
} block_header;

static uint8_t *heap = (uint8_t*)HEAP_START;
static block_header* free_list = NULL;

void init_heap()
{
  free_list = (block_header*)heap;
  free_list->size = HEAP_SIZE - sizeof(block_header);
  free_list->next = NULL;
  free_list->free = 1;
}

void* malloc(size_t size)
{
  block_header* current = free_list;
  
   while (current != NULL) {
        if (current->free && current->size >= size) {
            // If there's enough space to split the block
            if (current->size >= size + sizeof(block_header) + 8) {
                block_header* new_block = (block_header*)((uint8_t*)current + sizeof(block_header) + size);
                new_block->size = current->size - size - sizeof(block_header);
                new_block->free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->free = 0;
            return (void*)((uint8_t*)current + sizeof(block_header));
        }

        current = current->next;
    }

    return NULL; // Out of memory

  /*
    if (heap_used + size > HEAP_SIZE)
    {
        return NULL;
    }
    
    void* ptr = heap + heap_used;
    heap_used += size;
    return ptr; */
}

void free(void* ptr) {
    if (!ptr) return;

    block_header* block = (block_header*)((uint8_t*)ptr - sizeof(block_header));
    block->free = 1;

    // Optional: coalesce with next if it's free
    if (block->next && block->next->free) {
        block->size += sizeof(block_header) + block->next->size;
        block->next = block->next->next;
    }
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    block_header* old_block = (block_header*)((uint8_t*)ptr - sizeof(block_header));
    if (old_block->size >= size) {
        return ptr;
    }

    void* new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    // Copy old contents to new block (only up to the smaller of old/new size)
    size_t copy_size = old_block->size < size ? old_block->size : size;
    for (size_t i = 0; i < copy_size; i++) {
        ((uint8_t*)new_ptr)[i] = ((uint8_t*)ptr)[i];
    }

    free(ptr);
    return new_ptr;
}

