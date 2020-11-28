#include <Kernel/IDT.h>
#include <Kernel/Registers32.h>

void SyscallHandler(struct Registers32 Regs)
{
	((char*)0xB8000)[4] = 'S';
}

void Syscalls_Setup(void)
{
	/* Set our handler in the IDT handlers list
	 */
	SetInterruptHandler(HANDLER_SYSCALLS, (void (*)(void))SyscallHandler);
}
