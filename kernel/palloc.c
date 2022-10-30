#include <palloc.h>

page_t lastFreePage = 0;

page_t palloc(void) {
  //TODO: Tree
  //TODO: palloc multiple pages
  return lastFreePage++;
}

u64 pref(page_t page) {
  //TODO: Implement with the tree
  return 1;
}

void pfree(page_t page) {
  //TODO: Implement with the tree
}

void palloc_init(void) {
  //A: Claim first pages //TODO: Optimize
  page_t usedPages = CEIL(KSEND - KSSTART, PAGE_SIZE);
  printf("[palloc_init] usedPages=%X\n", usedPages);
  for (u64 i = 0; i < usedPages; i++) palloc();
  printf("[palloc_init] DONE lastFreePage=%X\n", lastFreePage);
}
