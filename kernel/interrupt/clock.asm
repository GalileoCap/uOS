%include "defines.mac"
%include "macros.mac"

extern pic_eoi
global clock_isr

section .text
clock_isr:
  pushaq
  cld

  ; TODO: Clock stuff

  mov dl, 32
  call pic_eoi
  popaq
  iretq
