%include "defines.mac"
%include "macros.mac"

global clock_isr
global keyboard_isr

clock_isr:
  pushaq
  cld

  ; TODO: Clock stuff

  releasePic 32
  popaq
  iretq

keyboard_isr:
  pushaq
  cld

  ; Read keycode
  in al, 0x60
  mov r12b, al
  releasePic 33

  ; TODO: Handle keycode
  mov rbx, 0xFFFFFa00000B8000
  mov [rbx], r12b

  popaq
  iretq
