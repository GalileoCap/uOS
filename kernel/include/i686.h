#ifndef __i686_H__
#define __i686_H__

#include <utils.h>

ALWAYS_INLINE void lcr3(u64 cr3);
ALWAYS_INLINE u64 rcr3(void);
ALWAYS_INLINE void tlbflush(void);

ALWAYS_INLINE void io_wait(void);
ALWAYS_INLINE u8 inb(u16 port);
ALWAYS_INLINE void outb(u16 port, u8 value);
ALWAYS_INLINE u32 inw(u16 port);
ALWAYS_INLINE void outw(u16 port, u32 value);

ALWAYS_INLINE void cli(void);
ALWAYS_INLINE void sti(void);

ALWAYS_INLINE void _break(u64 rax, u64 rbx, u64 rcx);

ALWAYS_INLINE void lcr3(u64 cr3) {
  __asm __volatile(
    "mov rax, %[cr3]\n\t"
    "mov cr3, rax\n\t"
    : : [cr3] "r" (cr3)
    : "rax"
  );
}

ALWAYS_INLINE u64 rcr3(void) {
  u64 cr3;
  __asm __volatile(
    "movq rax, cr3\n\t"
    "movq %[cr3], rax\n\t"
    : [cr3] "=r" (cr3)
    : : "rax"
  );
  return cr3;
}

ALWAYS_INLINE void tlbflush(void) {
  __asm(
    "mov rax, cr3\n\t"
    "mov cr3, rax\n\t"
    :
    :
    : "rax"
  );
}

ALWAYS_INLINE void io_wait(void) {
  outb(0x80, 0);
}

ALWAYS_INLINE u8 inb(u16 port) {
  u8 res;
  __asm __volatile(
    "mov dx, %[port]\n\t"
    "in al, dx\n\t"
    "mov %[res], al\n\t"
    : [res] "=r" (res)
    : [port] "r" (port)
    : "al", "dx"
  );
  return res;
}

ALWAYS_INLINE void outb(u16 port, u8 value) {
  __asm __volatile(
    "mov dx, %[port]\n\t"
    "mov al, %[value]\n\t"
    "out dx, al\n\t"
    "mov dx, 0x80\n\t"
    "mov al, 0\n\t"
    "out dx, al\n\t" //A: Always io_wait
    : : [port] "r" (port), [value] "r" (value)
    : "al", "dx"
  );
}

ALWAYS_INLINE u32 inw(u16 port) {
  u32 res;
  __asm __volatile(
    "mov dx, %[port]\n\t"
    "in eax, dx\n\t"
    "mov %[res], eax\n\t"
    : [res] "=r" (res)
    : [port] "r" (port)
    : "eax", "dx"
  );
  return res;
}

ALWAYS_INLINE void outw(u16 port, u32 value) {
  __asm __volatile(
    "mov dx, %[port]\n\t"
    "mov eax, %[value]\n\t"
    "out dx, eax\n\t"
    "mov dx, 0x80\n\t"
    "mov al, 0\n\t"
    "out dx, al\n\t" //A: Always io_wait
    : : [port] "r" (port), [value] "r" (value)
    : "eax", "dx"
  );
}

ALWAYS_INLINE void cli(void) {
  __asm __volatile("cli\n\t");
}

ALWAYS_INLINE void sti(void) {
  __asm __volatile("sti\n\t");
}

ALWAYS_INLINE void _break(u64 rax, u64 rbx, u64 rcx) {
  __asm __volatile(
    "mov rax, %[rax]\n\t"
    "mov rbx, %[rbx]\n\t"
    "mov rcx, %[rcx]\n\t"
    "xchg bx, bx\n\t"
    : : [rax] "r" (rax), [rbx] "r" (rbx), [rcx] "r" (rcx)
    : "rax", "rbx", "rcx"
  );
}

#endif //  __i686_H__
