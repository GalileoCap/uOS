#ifndef __PALLOC_H__
#define __PALLOC_H__

#include <mmu.h>
#include <utils.h>

page_t palloc(void);
u64 pref(page_t page);
u64 pfree(page_t page);

errno_t palloc_init(void);

#endif // __PALLOC_H__
