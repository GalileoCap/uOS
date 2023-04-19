#include <idt.h>

__attribute__((aligned(0x10)))
struct idt_entry IDT[256] = {0};
struct idt_desc IDT_DESC = {
  .size = (u16)sizeof(struct idt_entry) * 256 - 1, 
  .offset = (vaddr_t)&IDT,
};

struct tss *GDT_TSS;
struct ist IST;
