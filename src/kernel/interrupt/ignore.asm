%include "defines.mac"
%include "macros.mac"

extern pic_eoi
global ignore_isr

section .data:
ignore_isr:
%assign i 0
%rep 256
  dq ignore_isr%+i
%assign i i+1
%endrep

section .text
%assign i 0
%rep 256

global ignore_isr%+i
ignore_isr%+i:
  pushaq
  cld

  releasePic i

  popaq
  iretq
%assign i i+1
%endrep
