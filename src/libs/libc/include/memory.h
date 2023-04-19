#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <defines.h>

#ifndef HEAP_ALIGN
#define HEAP_ALIGN 0x10
#endif

#ifndef FIXBINS_COUNT
#define FIXBINS_COUNT (PAGE_SIZE / HEAP_ALIGN)
#endif

#ifndef VARBINS_COUNT
#define VARBINS_COUNT 1
#endif

void* malloc(size_t bytes);
void* calloc(size_t count, size_t bytes);
void free(void *ptr);

void heap_registerBin(size_t bytes);

void memcpy(void *dst, void *src, size_t count);
void memset(void *dst, u8 val, size_t count);

#ifdef KERNEL

#include <mmu.h>

#endif //KERNEL

#endif // __MEMORY_H__
