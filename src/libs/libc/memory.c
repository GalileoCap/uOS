#include <memory.h>
#ifdef KERNEL
#include <mmu.h>
#include <utils.h>
#endif // KERNEL

/************************************************************/
/* S: Utils *************************************************/

void memset(char *at, char value, size_t count) {
  for (size_t i = 0; i < count; i++)
    at[i] = value;
}

/************************************************************/
/* S: Malloc ************************************************/

void *heapStart = NULL,
     *heapEnd,
     *nextFree;

void expandHeap(size_t bytes) {
  size_t expandSz = ALIGN_UP(bytes, PAGE_SIZE);

  if (heapStart == NULL) { // The first time, claim a page
#ifndef KERNEL
  //TODO: User syscall
#else
    heapStart = (void*)KSEND; 
    heapEnd = heapStart + expandSz;
    KSEND += expandSz;
#endif

    nextFree = heapStart;
  }

  if ((nextFree + bytes) > heapEnd) {
#ifndef KERNEL
  //TODO: User syscall
#else
    mmu_map((vaddr_t)heapEnd, expandSz, PT_RW | PT_P); //TODO: attr
#endif

    heapEnd += expandSz;
  }
}

void* getNextFree(size_t bytes) {
  bytes = ALIGN_UP(bytes, HEAP_ALIGN);
  expandHeap(bytes);

  void *res = nextFree;
  nextFree += bytes;

  return res;
}

void* malloc(size_t bytes) {
  return getNextFree(bytes);
}

void* calloc(size_t n, size_t bytes) {
  return malloc(n * bytes); //TODO: Check for overflows
}

void free(void *ptr) {
}
  
