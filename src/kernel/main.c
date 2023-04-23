#include <serial.h>
#include <mmu.h>
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

  u64 *foo, *bar, *baz;
  foo = calloc(1, sizeof(u64));
  bar = calloc(5, sizeof(u64));
  baz = calloc(1, sizeof(u64));
  printf("A: %p %p %p\n", foo, bar, baz);
  free(foo); free(bar); free(baz);

  foo = calloc(1, sizeof(u64));
  bar = calloc(5, sizeof(u64));
  baz = calloc(1, sizeof(u64));
  printf("B: %p %p %p\n", foo, bar, baz);
  free(foo); free(bar); free(baz);

  foo = calloc(2, sizeof(u64));
  foo[0] = 0x600DF337;
  free(foo);
  bar = calloc(5, sizeof(u64));
  bar[1] = 0x600DB007;
  free(bar);
  baz = calloc(2, sizeof(u64));
  printf("C: %p %p %p\n", foo, bar, baz);
  printf("D: %X %X\n", baz[0], baz[1]);

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
