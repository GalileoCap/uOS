#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <defines.h>

#define HEAP_ALIGN 0x10

void* operator new(size_t size);
void* operator new[](size_t size); 
void operator delete(void *ptr);
void operator delete[](void *ptr); 
void operator delete(void *ptr, unsigned long s);
void operator delete[](void *ptr, unsigned long s);

void memset(char *at, char value, size_t count);
void memcpy(char *from, char *to, size_t count);

void* malloc(size_t bytes);
void* zalloc(size_t bytes);
void* calloc(size_t n, size_t bytes);
//void* realloc(void *ptr, size_t newBytes); //TODO

void free(void *ptr);

#endif // __MEMORY_H__
