#ifndef MEMORY_HEADER_H
#define MEMORY_HEADER_H

void* mem_alloc(int length);
void* mem_realloc(void* addr, int new_length);
void mem_free(void* addr);
void mem_dbg();

#endif // MEMORY_HEADER_H
