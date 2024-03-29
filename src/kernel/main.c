#include <serial.h>
#include <mmu.h>
#include <idt.h>
#include <utils.h>

//U: Define global variables //NOTE: Their values get set somewhere else
paddr_t KSSTART;
paddr_t KSEND;
vaddr_t KSTACK_TOP;
vaddr_t KSTACK_BOTTOM;

void kmain(void *mbd, bool magicError) {
  if (magicError)
    panic("[kmain] Invalid boot magic mbd=%p\n", mbd); //TODO: Print more information

  if (serial_init(COM1) != EOK)
    panic("[kmain] Serial failed errno=%X\n", errno);
  printf(
    "[kmain] Testing format: %d %i %u %z %x %X %p %s %c %% %a\n",
    1, -1, -2, 1025 * 0x1000, 0x600DF337, 0x600DB007, kmain, "\"A string\"", 'G'
  );

  if (mmu_init((struct multiboot_info*)mbd) != EOK)
    panic("[kmain] MMU failed errno=%X\n", errno);

  if (idt_init() != EOK) 
    panic("[kmain] IDT failed errno=%X\n", errno);

  printf("[kmain] REACHED END\n");
  printf("[you] ");
  char c;
  while (true) {
    serial_gets(COM1, &c, 1);

    if (c < ' ' || c > '~')
      printf("^C\n[you] ");
    else
      printf("%c", c);
  }
}
