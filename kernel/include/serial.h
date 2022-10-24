#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <utils.h>

#define COM1 0x3F8
#define COM2 0x2F8

#define COM_DATA        0
#define COM_IRQED       1
#define COM_IRQID       2
#define COM_LINECTRL    3
#define COM_MODEMCTRL   4
#define COM_LINESTATUS  5
#define COM_MODEMSTATUS 6
#define COM_SCRATCH     7

#define COM_BITS5 0x00
#define COM_BITS6 0x01
#define COM_BITS7 0x02
#define COM_BITS8 0x03

#define COM_STOP1 0x00
#define COM_STOP2 0x04

#define COM_PARITY_NONE   0x00
#define COM_PARITY_ODD    0x08
#define COM_PARITY_EVEN   0x18
#define COM_PARITY_MARK   0x28
#define COM_PARITY_SPACE  0x38

#define COM_LINE_DR    (1 << 0)
#define COM_LINE_OE    (1 << 1)
#define COM_LINE_FE    (1 << 3)
#define COM_LINE_BI    (1 << 4)
#define COM_LINE_TRHE  (1 << 5)
#define COM_LINE_TEMT  (1 << 6)
#define COM_LINE_ERROR (1 << 7)

//TODO: IRQED

//TODO: MODEM

/*
 * Returns EOK on success, EINVAL if the port is wrong, or ENODEV if the device isn't responsive
 */
errno_t serial_init(u16 port);

/*
 * Returns the number of bytes actually read/written
 * Sets errno to EOK on success, EINVAL if the port is wrong, or ENODEV if the device isn't responsive
 */
size_t serial_gets(u16 port, char *restrict buffer, size_t count);
size_t serial_puts(u16 port, const char *restrict msg, size_t count);

//TODO: Change into a character device

#endif // __SERIAL_H__
