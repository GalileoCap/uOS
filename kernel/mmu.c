#include <mmu.h>
#include <memory.h>
#include <palloc.h>
#include <i686.h>

size_t mmu_totalMemory; //U: Total size of the memory in bytes

void mmu_calcTotalMemory(const struct multiboot_info *mbd) {
  //A: Explore the BIOS memory map to find out the size of the memory
  if (!(mbd->flags >> 6 & 0x1)) panic("[mmu_calcTotalMemory] Invalid memory map mbd=%p\n", mbd);

  mmu_totalMemory = 0;
  struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry*)((u64)mbd->mmap_addr);
  for (u64 i = 0; i < (mbd->mmap_length / sizeof(struct multiboot_mmap_entry)); i++)
    mmu_totalMemory += mmap[i].len;

  printf("[mmu_calcTotalMemory] totalMemory=%z\n", mmu_totalMemory);
}

void mmu_removeIdentity(void) {
  __asm __volatile(
  "movq rax, cr3\n\t"
  "movq [rax], 0\n\t"
  : : : "rax"
  );
  printf("[mmu_removeIdentity]\n");
}

vaddr_t getVaddr(u64 pml4e, u64 pml3e, u64 pml2e, u64 pml1e) {
  return (vaddr_t)((0x0000ul << 48) | (pml4e << 39) | (pml3e << 30) | (pml2e << 21) | (pml1e << 12));
}

inline u64* getTable(u64 pml3e, u64 pml2e, u64 pml1e) {
  return (u64*)(getVaddr(RECURSE_ENTRY, pml3e, pml2e, pml1e) | (0xFFFFul << 48)); //TODO: Why do I need the 0xFFFFul? Crashes without it
}

errno_t mmu_init(const struct multiboot_info *mbd) {
  printf("[mmu_init]\n");
  mmu_calcTotalMemory(mbd);
  mmu_removeIdentity();
  palloc_init();

  mmu_map(getVaddr(USTACK_ENTRY, 0x1FF, 0x1FF, 0x1FF), PAGE_SIZE, PT_RW | PT_P); //A: Map the user stack
  mmu_map(getVaddr(KSTACK_ENTRY, 0x1FF, 0x1FF, 0x1FF), PAGE_SIZE, PT_RW | PT_P); //A: Map the kernel stack
  return_errno(EOK);
}

static void _mmapTMP(u64 pml3e, u64 pml2e, u64 pml1e, u64 entry, u16 attrs) { //TODO: Function name
  u64 *table = getTable(pml3e, pml2e, pml1e);
  if (!table[entry]) {
    table[entry] = PAGE_TO_ADDR(palloc()) | attrs;
    memset(getTable(pml2e, pml1e, entry), 0, PAGE_SIZE);
  }
}

void mmu_mapTo(vaddr_t vaddr, paddr_t to, u16 attrs) {
  /*printf("[mmu_mapTo] vaddr=%p, to=%p, attrs=%X\n", vaddr, to, attrs);*/
  u64 pml4e = PML4E(vaddr), pml3e = PML3E(vaddr), pml2e = PML2E(vaddr), pml1e = PML1E(vaddr);

  _mmapTMP(RECURSE_ENTRY, RECURSE_ENTRY, RECURSE_ENTRY, pml4e, attrs);
  _mmapTMP(RECURSE_ENTRY, RECURSE_ENTRY, pml4e, pml3e, attrs);
  _mmapTMP(RECURSE_ENTRY, pml4e, pml3e, pml2e, attrs);

  u64 *table = getTable(pml4e, pml3e, pml2e);
  table[pml1e] = to | attrs;
  pref(ADDR_TO_PAGE(to));

  tlbflush();
}

vaddr_t mmu_map(vaddr_t vaddr, size_t length, u16 attrs) {
  //TODO: Check if it's already mapped and find closest viable position
  //TODO: Add to process' list of claimed pages

  page_t pages = CEIL(length, PAGE_SIZE);
  /*printf("[mmu_map] vaddr=%p, length=%z(%X), attrs=%X\n", vaddr, length, pages, attrs);*/
  for (page_t i = 0; i < pages; i++)
    mmu_mapTo(vaddr + i * PAGE_SIZE, PAGE_TO_ADDR(palloc()), attrs); //TODO: palloc all pages at the same time

  return vaddr;
}

static bool _ummap(const u64 pml4e, const u64 pml3e, const u64 pml2e, const u64 pml1e) {
  u64 *table = getTable(pml4e, pml3e, pml2e);
  pfree(ADDR_TO_PAGE(table[pml1e])); //A: Free this physical page
  table[pml1e] = 0; //A: Clear the entry
  tlbflush();
  for (u16 i = 0; i < 512; i++) //A: Check for other entries in this table
    if (table[i]) return true;
  return false;
}

void ummap(vaddr_t vaddr) {
  //TODO: Ummap multiple at the same time
  //TODO: Check it's mapped
  u64 pml4e = PML4E(vaddr), pml3e = PML3E(vaddr), pml2e = PML2E(vaddr), pml1e = PML1E(vaddr);

  if (_ummap(pml4e, pml3e, pml2e, pml1e)) return;
  if (_ummap(RECURSE_ENTRY, pml4e, pml3e, pml2e)) return;
  if (_ummap(RECURSE_ENTRY, RECURSE_ENTRY, pml4e, pml3e)) return;
  _ummap(RECURSE_ENTRY, RECURSE_ENTRY, RECURSE_ENTRY, pml4e);
}
