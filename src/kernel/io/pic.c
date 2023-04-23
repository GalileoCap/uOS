#include <pic.h>
#include <i686.h>

#define PIC_EOI 0x20

void pic_eoi(u8 irq) {
  if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap(void) {
  u8 mask;

  mask = inb(PIC1_DATA);
  outb(PIC1_COMMAND, 0x11); 
  outb(PIC1_DATA, 0x20);
  outb(PIC1_DATA, 0x04);
  outb(PIC1_DATA, 0x01);
  outb(PIC1_DATA, mask);

  mask = inb(PIC2_DATA);
  outb(PIC2_COMMAND, 0x11); 
  outb(PIC2_DATA, 0x28);
  outb(PIC2_DATA, 0x02);
  outb(PIC2_DATA, 0x01);
  outb(PIC2_DATA, mask);
}

void pic_enable(void) {
  outb(PIC1_DATA, 0x00);
  outb(PIC2_DATA, 0x00);
}

void pic_disable(void) {
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);
}

void IRQ_setMask(unsigned char IRQline) {
  u16 port;
  u8 value;

  if(IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  outb(port, value);        
}
 
void IRQ_clearMask(unsigned char IRQline) {
  u16 port;
  u8 value;

  if(IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) & ~(1 << IRQline);
  outb(port, value);        
}
