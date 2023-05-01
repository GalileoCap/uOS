#ifndef __TMP_H__
#define __TMP_H__

#include <defines.h>

void* operator new(size_t size);
void* operator new[](size_t size); 
void operator delete(void *ptr);
void operator delete[](void *ptr); 
void operator delete(void *ptr, unsigned long s);
void operator delete[](void *ptr, unsigned long s);

#endif // __TMP_H__
