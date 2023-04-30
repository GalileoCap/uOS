#include <serial.h>
#include <mmu.h>
#include <idt.h>
#include <ata.h>
#include <vfs.h>
#include <utils.h>

//U: Define global variables //NOTE: Their values get set somewhere else
paddr_t KSSTART;
paddr_t KSEND;
vaddr_t KSTACK_TOP;
vaddr_t KSTACK_BOTTOM;

void kmain(void *mbd, bool magicError) asm("kmain");
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

  if (ata_init() != EOK) 
    panic("[kmain] ATA failed errno=%X\n", errno);

  /*fid_t fid = vfs_open("/dev0/README.md", VFS_MODE_READ | VFS_MODE_WRITE);*/
  fid_t fid = vfs_open("/dev0/subdir/test", VFS_MODE_READ | VFS_MODE_WRITE);
  if (fid == FID_INVALID) panic("[kmain] VFS_OPEN failed errno=%X\n", errno);
  
  char msg[12] = {'\0'};
  size_t count = vfs_read(fid, msg, sizeof(msg)-1);
  printf("[kmain] read %z with msg: \"%s\"\n", count, msg);

  /*char msg[] = "Ahoy there!";*/
  /*size_t count = vfs_read(fid, msg, sizeof(msg));*/
  /*printf("[kmain] wrote %z\n", count);*/

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
