#ifndef __PIC_H__
#define __PIC_H__

#include <utils.h>

#define PIC1  0x20
#define PIC2  0xA0
#define PIC1_COMMAND  PIC1
#define PIC1_DATA     (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA     (PIC2+1)

void pic_remap(void);
void pic_enable(void);
void pic_disable(void);

void pic_eoi(u8 irq);

#endif //  __PIC_H__
