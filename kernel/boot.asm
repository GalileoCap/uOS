; U: Constants for the multiboot header.
MBALIGN  equ  1 << 0            ; A: Align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; A: Provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; A: This is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; A: 'Magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; A: Checksum of above, to prove we are multiboot

; U: Multiboot header that marks the program as a kernel
section .multiboot
align 4
  dd MAGIC
  dd FLAGS
  dd CHECKSUM

section .text
bits 32
global _entry
_entry:
  jmp $
