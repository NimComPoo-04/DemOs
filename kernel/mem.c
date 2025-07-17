#include "mem.h"

// This is the worst allocator in the history of allocators

typedef struct mem_header_t
{
    char free;
    uint32_t size;
    char payload[];
} mem_header_t;

char *heap_start;
char *heap_end;

void init_allocator()
{
    heap_start = _KERNEL_END_;
    heap_end = _KERNEL_END_;
}

void *allocate(uint32_t size)
{
    // Search for a hole
    char *search_start = heap_start;
    mem_header_t *mem = search_start;

    while(mem->free != 1 && mem->size < size)
    {
        search_start += sizeof(mem_header_t) + mem->size;
        mem = search_start;
    }

    // Hole found lets goo
    if(search_start < heap_end)
    {
        mem->free = 0;
        return mem->payload;
    }
    else
    {
        mem_header_t *ptr = heap_end;
        heap_end += size + sizeof(mem_header_t);

        ptr->free = 0;
        ptr->size = size;

        return ptr->payload;
    }
}

// Deallocating is simple
void deallocate(void *p)
{
    struct mem_header_t *element = ((char *)p - sizeof(mem_header_t));
    element->free = 1;
}
