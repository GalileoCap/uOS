#ifndef __MMU_H__
#define __MMU_H__

#include <multiboot.h>
#include <utils.h>

#define ADDR_TO_PAGE(addr) (((page_t)addr) >> 12)
#define PAGE_TO_ADDR(addr) ((vaddr_t)(addr << 12))
#define CR3_ADDR(cr3) ((paddr_t)PAGE_TO_ADDR(ADDR_TO_PAGE(cr3)))
//TODO: Use CPUID MAXPHYADDR
//TODO: PML4/5 only if supported

#define PML4E(from) ((from >> 39) & 0x1FF)
#define PML3E(from) ((from >> 30) & 0x1FF)
#define PML2E(from) ((from >> 21) & 0x1FF)
#define PML1E(from) ((from >> 12) & 0x1FF)

#define PT_XD  (1 << 63)
#define PT_G   (1 << 8)
#define PT_PAT (1 << 7)
#define PT_D   (1 << 6)
#define PT_A   (1 << 5)
#define PT_PCD (1 << 4)
#define PT_PWT (1 << 3)
#define PT_US  (1 << 2)
#define PT_RW  (1 << 1)
#define PT_P   (1 << 0)

typedef u64 page_t;

extern size_t mmu_totalMemory;

errno_t mmu_mapTo(vaddr_t vaddr, paddr_t to, u16 attrs);
errno_t mmu_map(vaddr_t vaddr, size_t length, u16 attrs);
errno_t mmu_umap(vaddr_t vaddr, size_t length);

errno_t mmu_init(const struct multiboot_info *mbd);

void mmu_printCurrent(void);

#endif // __MMU_H__
