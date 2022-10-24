ENTRY(_entry)

LDSTART = 0xFFFFFa0000000000;
SECTIONS {
  /DISCARD/ : {
    *(.comment)
    *(.note.GNU-stack)
  }

  . = 1M;

  .boot : {
    *(.multiboot)
  }

  .btext : ALIGN(4K) {
    *boot.o(.text)
  }

  .brodata : ALIGN(4K) {
    *boot.o(.rodata)
  }


  .bdata : ALIGN(4K) {
    *boot.o(.data)
  }

  .bbss : ALIGN(4K) {
    *boot.o(COMMON)
    *boot.o(.bss)
  }

  . += LDSTART;

  .text ALIGN(4K) : AT(ADDR(.text) - LDSTART){
    *(.text)
  }

  .rodata ALIGN(4K) : AT(ADDR(.rodata) - LDSTART){
    *(.rodata)
  }

  .data ALIGN(4K): AT(ADDR(.data) - LDSTART){
    *(.data)
  }

  .bss ALIGN(4K) : AT(ADDR(.bss) - LDSTART){
    *(COMMON)
    *(.bss)
  }

  LDEND = ALIGN(4K) - LDSTART;
}
