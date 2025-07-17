#ifndef _MEM_H_
#define _MEM_H_

#include "kernel.h"

void init_allocator();

#define free_all() init_allocator()

void *allocate(uint32_t size);
void deallocate(void *p);

#endif
