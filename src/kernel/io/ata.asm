%include "defines.mac"
%include "macros.mac"

extern pic_eoi
global ata_isr

section .text
ata_isr:
  pushaq

  ; TODO: Send EOI https://wiki.osdev.org/ATA_PIO_Mode#IRQs

  popaq
  iretq
