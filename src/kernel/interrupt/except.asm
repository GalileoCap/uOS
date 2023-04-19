%include "defines.mac"
%include "macros.mac"

extern exception_handler
global except_isr

section .text
%macro ISRc 1
  push %1
  jmp exception
%endmacro

; U: ISR that pushes an exception code.
%macro ISRE 1
global _isr%1
_isr%1:
  ISRc %1
%endmacro

; U: ISR That doesn't push an exception code.
%macro ISRNE 1
global _isr%1
_isr%1:
  push 0
  ISRc %1
%endmacro

exception: 
  pushaq
  cld

  mov rdi, rsp
  call exception_handler

  popaq
  add rsp, 2 * 8 ; A: Remove codes
  iretq

ISRNE 0
ISRNE 1
ISRNE 2
ISRNE 3
ISRNE 4
ISRNE 5
ISRNE 6
ISRNE 7
ISRE  8
ISRNE 9
ISRE  10
ISRE  11
ISRE  12
ISRE  13
ISRE  14
ISRNE 15
ISRNE 16
ISRE  17
ISRNE 18
ISRNE 19
ISRNE 20
ISRNE 21
ISRNE 22
ISRNE 23
ISRNE 24
ISRNE 25
ISRNE 26
ISRNE 27
ISRNE 28
ISRNE 29
ISRE  30
ISRNE 31

section .data:
except_isr:
%assign i 0
%rep 32
  dq _isr%+i
%assign i i+1
%endrep
