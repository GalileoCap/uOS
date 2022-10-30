#include <memory.h>

void *nextFree = NULL, *heap_end = NULL;

void* malloc(size_t bytes) {
  bytes = ALIGN_UP(bytes, HEAP_ALIGN);

  if (nextFree == NULL) { //A: Need to init
#ifdef KERNEL
    nextFree = (void*)KSEND;
    KSEND = ALIGN_UP(KSEND + bytes, PAGE_SIZE);
    heap_end = (void*)KSEND;
#else
    //TODO: User syscall
#endif // KERNEL
  }

  void *res = nextFree;
  nextFree += bytes;

  if (res >= heap_end) { //A: Need to expand
#ifdef KERNEL
    mmu_map(ALIGN_UP((vaddr_t)res, PAGE_SIZE), ALIGN_UP(bytes, PAGE_SIZE), PT_RW | PT_P);
    heap_end += ALIGN_UP(bytes, PAGE_SIZE);
#else 
    //TODO: User syscall
#endif
  }

  return res;
}

void* calloc(size_t count, size_t bytes) {
  //TODO: Check for overflow
  bytes *= count;
  void *res = malloc(bytes);
  memset(res, 0, bytes);
  return res;
}

void free(void *ptr) {
  //TODO
}

void memcpy(void *_dst, void *_src, size_t count) {
  u8 *dst = _dst, *src = _src;
  for (u64 i = 0; i < count; i++)
    dst[i] = src[i];
}

void memset(void *_dst, u8 val, size_t count) {
  u8 *dst = _dst;
  for (u64 i = 0; i < count; i++)
    dst[i] = val;
}

