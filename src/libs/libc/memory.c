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

void memcpy(char *from, char *to, size_t count) {
  for (size_t i = 0; i < count; i++)
    to[i] = from[i];
}

/************************************************************/
/* S: Malloc ************************************************/

struct heapNode_t {
  struct heapNode_t *prev, *next;
  bool free;
  char data[];
};

struct heapNode_t *heapHead = NULL,
                  *heapTail = NULL;
void *heapEnd = NULL;

void* nodeData(struct heapNode_t *node) {
  return (void*)node + offsetof(struct heapNode_t, data);
}

size_t nodeSize(struct heapNode_t *node) {
  return (size_t)node->next - (size_t)nodeData(node);
}

struct heapNode_t* nodeFromData(void *ptr) {
  return (struct heapNode_t*)(ptr - offsetof(struct heapNode_t, data));
}

void expandHeap(size_t bytes) {
  size_t expandSz = ALIGN_UP(sizeof(struct heapNode_t) + bytes, PAGE_SIZE);

  if (heapHead == NULL) { // The first time, claim a page
#ifndef KERNEL
  //TODO: User syscall
#else
    heapHead = (struct heapNode_t*)KSEND; 
    heapEnd = (void*)heapHead + expandSz;
    KSEND = (paddr_t)heapEnd;
#endif

    heapHead->prev = NULL;
    heapHead->next = NULL;
    heapHead->free = true;
    heapTail = heapHead;
    /*printf("[expandHeap] INIT: %p %p %p\n", heapHead, heapTail, heapEnd);*/
  } else {
#ifndef KERNEL
  //TODO: User syscall
#else
    mmu_map((vaddr_t)heapEnd, expandSz, PT_RW | PT_P); //TODO: attr
#endif

    heapEnd += expandSz;
    /*printf("[expandHeap] EXPAND: %p %p %p\n", heapHead, heapTail, heapEnd);*/
  }
}

struct heapNode_t* getNextFreeFrom(struct heapNode_t *node) {
  while (node != NULL && !node->free)
    node = node->next;
  return node;
}

struct heapNode_t* getFirstFreeOfSize(size_t bytes) {
  struct heapNode_t *node = getNextFreeFrom(heapHead);
  while (node != NULL && nodeSize(node) < bytes)
    node = getNextFreeFrom(node);

  if (node == NULL) {
    expandHeap(bytes);
    node = heapTail;
  }

  if (nodeSize(node) > (bytes + sizeof(struct heapNode_t) + HEAP_ALIGN)) { // Split if large enough
    struct heapNode_t *newNode = (struct heapNode_t*)(nodeData(node) + bytes);
    newNode->prev = node;
    newNode->next = node->next;
    newNode->free = true;
    if (newNode->next != NULL)
      newNode->next->prev = newNode;

    node->next = newNode;
    if (node == heapTail)
      heapTail = newNode;
    /*printf("[splitNode] %p %p %p\n", node, newNode, heapTail);*/
  }

  return node;
}

void* malloc(size_t bytes) {
  bytes = ALIGN_UP(bytes, HEAP_ALIGN);

  struct heapNode_t *node = getFirstFreeOfSize(bytes);
  /*printf("[malloc] %p %x %p %p\n", node, node->free, node->prev, node->next, nodeData(node));*/
  node->free = false;

  return nodeData(node);
}

void* zalloc(size_t bytes) {
  void *res = malloc(bytes);
  memset((char*)res, 0, bytes);
  return res;
}

void* calloc(size_t n, size_t bytes) {
  return zalloc(n * bytes); //TODO: Check for overflows
}

void free(void *ptr) {
  return; // TODO: Fix
  struct heapNode_t *node = nodeFromData(ptr);
  /*printf("[free] %p %p %z\n", ptr, node, sizeof(struct heapNode_t));*/
  node->free = true;

  if (node->prev != NULL && node->prev->free) { // Merge back
    node->prev->next = node->next;
    if (node->next != NULL)
      node->next->prev = node->prev;
    node = node->prev; // To allow merging forward
  }

  if (node->next != NULL && node->next->free) { // Merge forward
    if (node->next == heapTail)
      heapTail = node;

    node->next = node->next->next;
    if (node->next != NULL)
      node->next->prev = node;
  }
}
