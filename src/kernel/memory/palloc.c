#include <palloc.h>

struct page_node {
  u64 refs;
};

struct page_node *page_list;
u64 totalPages;

page_t palloc(void) {
  //TODO: palloc multiple pages
  page_t i = 0;
  while (i < totalPages && page_list[i].refs != 0) i++;

  pref(i);
  return i;
}

u64 pref(page_t page) {
  return ++page_list[page].refs;
}

u64 pfree(page_t page) {
  return --page_list[page].refs;
}

errno_t palloc_init(void) {
  // Allocate the list
  totalPages = CEIL(mmu_totalMemory, PAGE_SIZE);
  page_list = (struct page_node*)calloc(totalPages, sizeof(struct page_node));
  printf(
    "[palloc_init] page_list=%p, totalPages=%X, size=%z\n",
    page_list, totalPages, totalPages * sizeof(struct page_node)
  );
  
  // Claim kernel pages
  page_t usedPages = CEIL(KSEND - KSSTART, PAGE_SIZE);
  printf("[palloc_init] usedPages=%X\n", usedPages);
  for (u64 i = 0; i < usedPages; i++)
    page_list[i].refs = 1;

  // Set all other pages as free
  for (u64 i = usedPages; i < totalPages; i++)
    page_list[i].refs = 0;

  return_errno(EOK)
}
