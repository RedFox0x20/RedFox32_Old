#include <Kernel/IDT.h>
#include <Kernel/Syscalls.h>
#include <Kernel/IO.h>

int KMain()
{
	IDT_Setup();
	Syscalls_Setup();
	asm volatile ("sti");
	asm volatile ("int $0x80");
	for(;;) asm volatile("hlt");
	return 0;
}

