# ifndef __EXCEPT_H__
# define __EXCEPT_H__

#include <utils.h>

struct except {
  u64 r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx, rbx, rax, ign0;
  u64 number, code;
  u64 rip, cs, rflags, rsp, ss;
} __attribute__((packed));

void exception_handler(const struct except *exception);

#endif // __EXCEPT_H__
