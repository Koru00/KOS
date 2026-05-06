// kernel/mm/pmm.c

#include <stddef.h>
#include <stdint.h>

typedef struct page {
    struct page *next;
} page_t;

static page_t *free_list = NULL;
static size_t free_pages = 0;

void pmm_free(void *p);

void pmm_init(uintptr_t start, uintptr_t end) {
    // Align start up to the nearest 4KB
    start = (start + 4095) & ~4095;
    
    for (uintptr_t p = start; p + 4096 <= end; p += 4096) {
        pmm_free((void*)p);
    }
}

void *pmm_alloc() {
    if (!free_list) return NULL;
    
    page_t *p = free_list;
    free_list = p->next;
    free_pages--;
    
    // Clear the page before returning (security/stability)
    
    // TODO Implement memset
    // memset(p, 0, 4096);
    return (void*)p;
}

void pmm_free(void *p) {
    page_t *page = (page_t*)p;
    page->next = free_list;
    free_list = page;
    free_pages++;
}
