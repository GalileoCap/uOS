#include <utils.h>

//U: Define global variables //NOTE: Their values get set somewhere else
paddr_t KSSTART;
paddr_t KSEND;
vaddr_t KSTACK_TOP;
vaddr_t KSTACK_BOTTOM;
paddr_t GDT_TSS;

void kmain(void *mbd, bool magicError) {
  while (true);
}
