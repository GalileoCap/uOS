#include <palloc.h>

struct page_node {
  u64 refs;
};

struct page_node *page_tree;
u64 leavesStart;

inline bool isLeaf(u64 idx) {
  return idx >= leavesStart;
}

inline u64 page2Idx(page_t page) {
  return leavesStart + page;
}

inline u64 idx2Page(u64 idx) {
  return idx - leavesStart;
}

inline u64 leftChild(u64 idx) {
  return 2 * idx + 1;
}

inline u64 rightChild(u64 idx) {
  return 2 * idx + 2;
}

inline u64 parent(u64 idx) {
  return (idx - 1) / 2;
}

page_t palloc(void) {
  //TODO: palloc multiple pages
  u64 idx = 0;
  if (page_tree[idx].refs == 0) panic("[palloc] full memory\n"); //A: Full memory

  while (!isLeaf(idx)) { //A: Until I reach a page
    page_tree[idx].refs--;
    if (!isLeaf(leftChild(idx))) {
      if (page_tree[leftChild(idx)].refs >= 1) idx = leftChild(idx);
      else /* (page_tree[rightChild(idx)].refs >= 1) */ idx = rightChild(idx);
    } else {
      if (page_tree[leftChild(idx)].refs == 0) idx = leftChild(idx);
      else /* (page_tree[rightChild(idx)].refs == 0) */ idx = rightChild(idx);
    }
  }
  
  pref(idx2Page(idx));
  return idx2Page(idx);
}

u64 pref(page_t page) {
  return ++page_tree[page2Idx(page)].refs;
}

void pfree(page_t page) {
  u64 idx = page2Idx(page);
  if ((--page_tree[idx].refs) == 0) do {
    idx = parent(idx);
    page_tree[idx].refs++;
  } while (idx != 0);
}

void palloc_init(void) {
  //TODO: Handle extra nodes
  //A: Allocate the tree
  u64 treeHeight = log2c(CEIL(mmu_totalMemory, PAGE_SIZE)) + 1;
  u64 nodes = pow2(treeHeight);
  page_tree = calloc(nodes, sizeof(struct page_node));
  printf("[palloc_init] %p, treeHeight=%X, nodes=%X, %z\n", page_tree, treeHeight, nodes, nodes * sizeof(struct page_node));
  
  //A: Set the tree with all pages set to free
  for (u64 height = 0; height < treeHeight; height++) {
    nodes /= 2;
    for (u64 i = pow2(height)-1; i < (pow2(height+1)-1); i++)
      page_tree[i].refs = (nodes > 1) ? nodes : 0; //A: Set the actual pages to free
  }
  leavesStart = pow2(treeHeight-1)-1;

  //A: Claim first pages //TODO: Optimize
  page_t usedPages = CEIL(KSEND - KSSTART, PAGE_SIZE);
  printf("[palloc_init] usedPages=%X\n", usedPages);
  for (u64 i = 0; i < usedPages; i++) palloc();
}
