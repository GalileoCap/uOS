#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <defines.h>
#include <memory.h>

extern paddr_t KSSTART;
extern paddr_t KSEND;

extern vaddr_t KSTACK_TOP;
extern vaddr_t KSTACK_BOTTOM;

extern paddr_t GDT_TSS;

#endif // __UTILS_H__
