#include <memory.h>
#include <stdio.h>

struct heap_block {
  size_t prevSize, size;
  struct heap_block *prevFree, *nextFree;
};

struct heap_bininfo {
  struct heap_block *firstFree;
};

struct heap_bininfo bins[FIXBINS_COUNT+1] = { 0 };
void *heap_end = NULL;

inline u64 binIdx(size_t bytes) {
  u64 res = (bytes / HEAP_ALIGN) - 1;
  return MIN(res, FIXBINS_COUNT); //A: If it's not a fixed size, send it to the variable size
}

struct heap_block* getNextBlock(struct heap_block *block, size_t size) {
  return (void*)block + size + offsetof(struct heap_block, prevFree);
}

void insertBlockToBin(struct heap_block *block) {
  block->nextFree = bins[binIdx(block->size)].firstFree;
  bins[binIdx(block->size)].firstFree = block;
  if (block->nextFree != NULL) block->nextFree->prevFree = block;
}

void removeBlockFromBin(struct heap_block *block) {
  struct heap_bininfo *bin = &bins[binIdx(block->size)];
  if (block == bin->firstFree) bin->firstFree = block->nextFree;
  else block->prevFree->nextFree = block->nextFree;
}

void splitBlock(struct heap_block *block, size_t bytes) {
  struct heap_block *nextBlock = getNextBlock(block, block->size),
                    *newBlock = getNextBlock(block, bytes);

  //A: Create a new block after this one
  newBlock->prevSize = bytes; //TODO: Not free
  newBlock->size = (size_t)nextBlock - (size_t)newBlock - offsetof(struct heap_block, nextFree); //TODO: Set free
  nextBlock->prevSize = newBlock->size;
  insertBlockToBin(newBlock);

  //A: Update this block
  removeBlockFromBin(block);
  block->size = bytes; //TODO: Set not free
  insertBlockToBin(block);

  return;
  //TODO?
  //A: Make sure to split into PAGE_SIZE or smaller
  if (newBlock->size > PAGE_SIZE)
    splitBlock(newBlock, PAGE_SIZE);
}

void newFirstFree(size_t bytes) {
  //A: Find an already free block to split
  //A: Search in fix size
  for (u64 idx = FIXBINS_COUNT-1; idx > binIdx(bytes); idx--) //A: Search form the largest to the smallest //TODO: Minimum size //TODO: Small->large?
    if (bins[idx].firstFree != NULL) { //A: Has a free block
      splitBlock(bins[idx].firstFree, bytes);
      return;
    }

  //A: Search in var size
  for (struct heap_block *block = bins[FIXBINS_COUNT].firstFree; block != NULL; block = block->nextFree) {
    if (block->size >= bytes) { //TODO: Minimum size
      splitBlock(block, bytes);
      return;
    }
  }

  //A: Need to expand
  size_t expandSz = ALIGN_UP(bytes + sizeof(struct heap_block), PAGE_SIZE);
#ifdef KERNEL
  if (heap_end == NULL) { //A: The first time before palloc is ready, claim these pages
    heap_end = (void*)KSEND + sizeof(size_t);
    *(size_t*)(heap_end - sizeof(size_t)) = 0;
    KSEND += expandSz;
  } else {
    mmu_map((vaddr_t)heap_end, expandSz, PT_RW | PT_P);
  }
#else 
  //TODO: User syscall
#endif
  struct heap_block *block = heap_end - sizeof(size_t);
  block->size = expandSz;
  block->nextFree = NULL;
  heap_end += expandSz;
  insertBlockToBin(block);
  splitBlock(block, bytes);
}

void* malloc(size_t bytes) {
  bytes = ALIGN_UP(bytes, HEAP_ALIGN);
  struct heap_bininfo *bin = &bins[binIdx(bytes)];
  if (bin->firstFree == NULL) newFirstFree(bytes);

  struct heap_block *res = bin->firstFree;
  bin->firstFree = bin->firstFree->nextFree;

  return &res->prevFree; //A: Point to the data field
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

