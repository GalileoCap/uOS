#ifndef __PALLOC_H__

#include <mmu.h>
#include <utils.h>

page_t palloc(void);
u64 pref(page_t page);
void pfree(page_t page);

void palloc_init(void);

#endif // __PALLOC_H__ 
