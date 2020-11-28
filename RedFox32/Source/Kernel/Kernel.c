#include <Kernel/IDT.h>
#include <Kernel/IO.h>

int KMain()
{
	IDT_Setup();
	outb(0x21, 0xFD);
	asm volatile ("sti");
	while (1) { asm volatile ("hlt"); }
	return 0;
}

