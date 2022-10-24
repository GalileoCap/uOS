#include <serial.h>
#include <i686.h>
#include <utils.h>

//U: Define global variables //NOTE: Their values get set somewhere else
paddr_t KSSTART;
paddr_t KSEND;
vaddr_t KSTACK_TOP;
vaddr_t KSTACK_BOTTOM;
paddr_t GDT_TSS;

void kmain(void *mbd, bool magicError) {
  if (magicError) _break(-1, 0, 0); //TODO: Panic

  if (serial_init(COM1) != EOK) _break(-1, 1, errno); //TODO: Panic
  serial_puts(COM1, "Hello there!", 12);

  while (true);
}
