#include <serial.h>
#include <i686.h>

inline bool checkPort(u16 port) {
  return (port == COM1) || (port == COM2);
}

inline bool received(u16 port) {
  return FLAGS_SET(inb(port + COM_LINESTATUS), COM_LINE_DR);
}

inline bool isEmpty(u16 port) {
  return FLAGS_SET(inb(port + COM_LINESTATUS), COM_LINE_TRHE);
}

char serial_get(u16 port) {
  while (!received(port));
  //TODO: Don't hang
  return inb(port);
}

size_t serial_gets(u16 port, char *restrict buffer, size_t count) {
  if (!checkPort(port)) return_errno(EINVAL);

  for (size_t i = 0; i < count; i++)
    buffer[i] = serial_get(port);

  return count;
}

void serial_put(u16 port, char c) {
  while (!isEmpty(port));
  //TODO: Don't hang
  outb(port, c);
}

size_t serial_puts(u16 port, const char *restrict buffer, size_t count) {
  if (!checkPort(port)) return_errno(EINVAL);

  for (size_t i = 0; i < count; i++)
    serial_put(port, buffer[i]);

  return count;
}

errno_t serial_init(u16 port) {
  outb(port + COM_IRQED, 0x00); //A: Disable interrupts
  outb(port + COM_LINECTRL, 0x80); //A: Enable DLAB (set baud rate divisor)
  outb(port + COM_DATA, 0x03); //A: Set divisor to 3 (lo-byte) 38400 baud
  outb(port + COM_IRQED, 0x00); //A: Divisor (hi-byte)
  outb(port + COM_LINECTRL, COM_BITS8 | COM_PARITY_NONE | COM_STOP1); //A: 7-bits, no parity, one stop bit
  outb(port + COM_IRQID, 0xC7); //A: Enable FIFO, clear them, with 14-byte threshold
  outb(port + COM_MODEMCTRL, 0x0B); //A: IRQs enabled, RTS/DSR set

  //A: Test serial chip (send byte 0xAE and check if serial returns same byte)
  char test = 0xAE;
  outb(port + COM_MODEMCTRL, 0x1E); //A: Set in loopback mode, test the serial chip
  serial_put(port, test);
  if(serial_get(port) != test) return_errno(ENODEV);
  outb(port + COM_MODEMCTRL, 0x0F); //A: It's not faulty set it in normal operation mode (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)

  return_errno(EOK);
}

