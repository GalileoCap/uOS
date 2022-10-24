%include "defines.mac"
%include "macros.mac"

extern kmain

extern LDSTART
extern LDEND
extern KSSTART
extern KSEND

extern GDT_TSS

global _entry

global GDT.Null
global GDT.CodeSys
global GDT.DataSys
global GDT.CodeUser
global GDT.DataUser
global GDT.TSS
global GDT.Descriptor

; U: Access bits
PRESENT        equ 1 << 7
USER           equ 3 << 5 
NOT_SYS        equ 1 << 4 
EXEC           equ 1 << 3 
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0 

; U: Flags bits
GRAN_4K       equ 1000b << 4
SZ_32         equ 0100b << 4
LONG_MODE     equ 0010b << 4

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
BITS 32
_entry:
  ; NOTE: The bootloader left us on 32-bit protected mode without paging

  ; A: Set the stack
  mov esp, _STACKTOP
  mov ebp, _STACKTOP

  ; A: Save multiboot info
  mov ecx, 0
  push ecx ; NOTE: Padding because we're going to pop 64-bits
  push eax
  push ecx
  push ebx

  ; A: Load the GDT
  lgdt [GDT.Descriptor]
  mov ax, GDTDS_R0
  mov ds, ax
  mov es, ax
  mov gs, ax
  mov fs, ax
  mov ss, ax

  ; TODO: Check if long mode is available

  call setup_paging
  mov cr3, eax ; NOTE: Causes tlbflush //TODO: Attributes

  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  mov eax, cr0
  or eax, 1 << 31
  mov cr0, eax

  jmp GDTCS_R0:long_mode
.end:

setup_paging: ; U: Maps the first 32MiB of memory as identity and on the start of the higher-half with recursive paging ; TODO: Map up to _KSEND
  mov edx, 0
  mov edi, [_KSEND]
  push edi
  lea eax, [edi + PAGE_SIZE + PT_RW + PT_P] ; TODO: Attributes
  mov ecx, 1

  ; A: First entry on the PML4
  call fill_page_table

  ; A: First entry on the PML3
  call fill_page_table

  ; A: Sixteen PML2's for each PML3
  shl ecx, 4
  call fill_page_table

  ; A: Full PML1's for each PML2
  and eax, PT_RW | PT_P
  shl ecx, 9
  call fill_page_table

  mov [_KSEND], edi ; A: Update _KSEND
  pop eax ; A: Recover the CR3

  ; A: Copy the first PML4 entry to the last
  lea ebx, [eax + PAGE_SIZE + PT_RW + PT_P] ; TODO: Attributes
  mov [eax + KSSTART_ENTRY * 8], ebx
  mov [eax + KSSTART_ENTRY * 8 + 4], edx

  ; A: Recursive paging
  lea ebx, [eax + PT_RW + PT_P] ; TODO: Attributes
  mov [eax + RECURSE_ENTRY * 8], ebx
  mov [eax + RECURSE_ENTRY * 8 + 4], edx

  ret

fill_page_table:
  lea esi, [ecx * 8]
  add esi, edi
  .loop:
    mov [edi], eax
    mov [edi + 4], edx
    add eax, PAGE_SIZE
    add edi, 8
    cmp edi, esi
    jnz .loop
  .loop_zero:
    test edi, PAGE_SIZE - 1
    jz .end
    mov dword [edi], 0
    add edi, 4
    jmp .loop_zero
.end:
  ret

BITS 64
long_mode:
  ; A: Recover multiboot result
  pop rdi
  pop rsi
  xor rsi, MULTIBOOT_MAGIC ; A: 0 if they match

  ; A: Move the stack to the higher-half
  add rsp, [_KSSTART]
  add rbp, [_KSSTART]

  ; A: Update KSSTART
  mov rax, [_KSSTART]
  mov rbx, KSSTART
  mov [rbx], rax

  ; A: Update KSEND
  mov rax, [_KSEND]
  add rax, [_KSSTART]
  mov rbx, KSEND
  mov [rbx], rax

  ; A: Update the GDT
  mov rax, GDT
  add rax, [_KSSTART]
  mov [GDT.Descriptor + 0x2], rax
  lgdt [GDT.Descriptor]

  ; A: Update GDT_TSS
  mov rax, GDT
  add rax, GDT.TSS
  add rax, [_KSSTART]
  mov rbx, GDT_TSS
  mov [rbx], rax

  add rdi, [_KSSTART]
  mov rax, kmain
  call rax

.end:
  jmp $

; U: Stack
section .bss
align 16
_STACKBOTTOM:
resb 0x4000 ; A: 4 pages = 16 KiB
_STACKTOP:

section .data
_KSSTART: dq LDSTART
_KSEND: dq LDEND

GDT:
.Null: equ $ - GDT ; U: Null selector
  dq 0
.CodeSys: equ $ - GDT 
  dd 0xFFFF                                   ; U: Limit & Base (low, bits 0-15) ; NOTE: Ignored in long mode
  db 0                                        ; U: Base (mid, bits 16-23) ; NOTE: Ignored in long mode
  db PRESENT | NOT_SYS | EXEC | RW            ; U: Access
  db GRAN_4K | LONG_MODE | 0xF                ; U: Flags & Limit (high, bits 16-19)
  db 0                                        ; U: Base (high, bits 24-31) ; NOTE: Ignored in long mode
.DataSys: equ $ - GDT
  dd 0xFFFF
  db 0
  db PRESENT | NOT_SYS | RW
  db GRAN_4K | SZ_32 | 0xF
  db 0
.CodeUsr: equ $ - GDT
  dd 0xFFFF
  db 0
  db PRESENT | USER | NOT_SYS | EXEC | RW
  db GRAN_4K | LONG_MODE | 0xF
  db 0
.DataUsr: equ $ - GDT
  dd 0xFFFF
  db 0
  db PRESENT | USER | NOT_SYS | RW
  db GRAN_4K | SZ_32 | 0xF
  db 0
.TSS: equ $ - GDT
  dq 0x0 ; A: Left for the IDT_init to fill
  dq 0x0
GDT.Descriptor:
GDT.Size:  dw $ - GDT - 1
GDT.Base: dq GDT
