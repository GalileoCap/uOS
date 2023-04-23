%include "defines.mac"
%include "macros.mac"

extern pic_eoi
global clock_isr
global keyboard_isr

section .text
clock_isr:
  pushaq
  cld

  ; TODO: Clock stuff

  ; Release the PIC
  mov dl, 32
  call pic_eoi

  popaq
  iretq

keyboard_isr:
  pushaq
  cld

  ; Read keycode
  in al, 0x60
  mov r12b, al

  ; Release the PIC
  mov dl, 33
  call pic_eoi

  ; TODO: Handle keycode
  mov rbx, 0xFFFFFa00000B8000
  mov [rbx], r12b

  popaq
  iretq
