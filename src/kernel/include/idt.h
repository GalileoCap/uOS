#ifndef __IDT_H__
#define __IDT_H__

#include <utils.h>

#define GDT_TYPE_INTERRUPT 0x0E
#define GDT_TYPE_TRAP      0x0F
#define GDT_PERM_USER      0x60
#define GDT_PERM_KERNEL    0x00
#define GDT_PRESENT        0x80

struct idt_desc {
  u16 size; 
  vaddr_t offset; 
} __attribute__((packed));

struct idt_entry {
  u16 offset_0_15; 
  u16 sel; //U: Code selector in the GDT
  u8  ist : 3; //U: Interrupt Stack Table offset
  u8  rsv0 : 5; //U: Reserved
  u8  flags;
  u16 offset_16_31;
  u32 offset_32_63;
  u32 rsv1;
} __attribute__((__packed__, aligned (8)));

struct tss {
  u16 limitL,
      baseL;
  u8  baseM0,
      access,
      limitH : 4,
      flags : 4,
      baseM1;
  u32 baseH,
      resv0;
} __attribute__((__packed__));

struct ist {
  u64 resv3,
      rsp0,
      rsp1,
      rsp2,
      resv2,
      ist1,
      ist2,
      ist3,
      ist4,
      ist5,
      ist6,
      ist7,
      resv1;
  u16 resv0,
      iobp;
  u32 ign0;
  u64 iobp0, iobp1, iobp2, iobp3;
} __attribute__((__packed__));

#endif //  __IDT_H__
