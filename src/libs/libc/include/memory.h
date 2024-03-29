#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <defines.h>

#define HEAP_ALIGN 0x10

void memset(char *at, char value, size_t count);

void* malloc(size_t bytes);
void* calloc(size_t n, size_t bytes);
//void* realloc(void *ptr, size_t newBytes); //TODO

void free(void *ptr);

#endif // __MEMORY_H__
