#include <idt.h>
#include <pic.h>
#include <i686.h>

__attribute__((aligned(0x10)))
struct idt_entry IDT[256] = {0};
struct idt_desc IDT_DESC = {
  .size = (u16)sizeof(struct idt_entry) * 256 - 1, 
  .offset = (vaddr_t)&IDT,
};

struct tss *GDT_TSS;
struct ist IST;

extern void* ignore_isr[];
extern void* except_isr[];
extern void* clock_isr;
extern void* keyboard_isr;

void idt_register(u8 vector, void *isr, u8 flags) {
  printf("[idt_register] vector=%u, isr=%p, flags=%X\n", vector, isr, flags);
  //TODO: PIC mask
  IDT[vector] = (struct idt_entry){
    .offset_0_15 = (u64)isr & 0xFFFF,
    .sel = GDTCS_R0, //TODO: User/Kernel
    .ist = 0,
    .flags = flags | GDT_PRESENT,
    .offset_16_31 = ((u64)isr >> 16) & 0xFFFF,
    .offset_32_63 = ((u64)isr >> 32) & 0xFFFFFFFFF,
    .rsv1 = 0
  };
}

errno_t idt_init(void) {
  //A: Exceptions
  for (u8 i = 0; i < 32; i++)
    idt_register(i, except_isr[i], GDT_TYPE_TRAP | GDT_PERM_KERNEL);

  // By default ignore all other interruptions
  for (u64 i = 32; i < 256; i++)
    idt_register(i, ignore_isr[i], GDT_TYPE_TRAP | GDT_PERM_KERNEL);

  //A: I/O
  idt_register(32, &clock_isr, GDT_TYPE_INTERRUPT | GDT_PERM_KERNEL); //A: Clock
  idt_register(33, &keyboard_isr, GDT_TYPE_INTERRUPT | GDT_PERM_KERNEL); //A: Keyboard

  /*INTERRUPTR0(46); //A: Primary ATA Hard Disk*/
  /*INTERRUPTR0(47); //A: Secondary ATA Hard Disk*/

  //A: Syscalls
  /*INTERRUPTR0(128); //A: 0x80*/

  //A: Load the IDT
  __asm __volatile(
    "lidt %[IDT_DESC]\n\t"
    :
    : [IDT_DESC] "m" (IDT_DESC)
  );
  printf("[idt_init] load\n");

  //A: Setup the TSS
  //A: Set the iobp with every permission //TODO: Change it with user
  IST.iobp = (u16)((u64)&IST.iobp0 - (u64)&IST);
  IST.iobp0 = 0; IST.iobp1 = 0; IST.iobp2 = 0; IST.iobp3 = 0; 
  //TODO: Setup the IST stacks

  GDT_TSS->baseH = (u64)(&IST) >> 32;
  GDT_TSS->baseM1 = ((u64)(&IST) >> 24) & 0xFF;
  GDT_TSS->baseM0 = ((u64)(&IST) >> 16) & 0xFF;
  GDT_TSS->baseL = (u64)(&IST) & 0xFFFF;
  GDT_TSS->limitH = ((sizeof(struct ist) - 1) >> 16) & 0xF;
  GDT_TSS->limitL = (sizeof(struct ist) - 1) & 0xFFFF;
  GDT_TSS->access = 0x89;
  GDT_TSS->flags = 0x0;

  //TODO: Check GDT_TSS is in range
  __asm __volatile(
    "mov rax, 0x28\n\t"
    "ltr ax\n\t"
    : : : "rax"
  );
  printf("[idt_init] load tss\n");

  pic_remap();
  pic_enable();
  /*IRQ_setMask(0);*/
  /*IRQ_setMask(1);*/
  /*IRQ_setMask(2);*/
  /*IRQ_setMask(3);*/
  /*IRQ_setMask(4);*/
  /*IRQ_setMask(5);*/
  /*IRQ_setMask(6);*/
  /*IRQ_setMask(7);*/

  sti();
  printf("[idt_init] sti\n");
  return_errno(EOK);
}
