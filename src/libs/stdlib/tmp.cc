#include "tmp.h"
#include <memory.h>

/************************************************************/
/* S: RTL ***************************************************/
//TODO: Move

void* operator new(size_t size) { return zalloc(size); }
void* operator new[](size_t size) { return zalloc(size); }
void operator delete(void *ptr) { return free(ptr); }
void operator delete[](void *ptr) { return free(ptr); }

// SEE: https://en.cppreference.com/w/cpp/memory/new/operator_delete
void operator delete(void *ptr, unsigned long s) { return delete(ptr); }
void operator delete[](void *ptr, unsigned long s) { return delete[](ptr); };
