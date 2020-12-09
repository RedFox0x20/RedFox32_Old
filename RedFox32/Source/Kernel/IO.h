#ifndef KERNEL_STAGE2_IO_H
#define KERNEL_STAGE2_IO_H

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);
unsigned char inb(unsigned short port);

#endif
