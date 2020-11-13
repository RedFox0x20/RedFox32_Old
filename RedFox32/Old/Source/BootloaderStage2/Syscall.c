#include <BootloaderStage2/Interrupts.h>
#include <BootloaderStage2/Video.h>


unsigned int Syscalls[0x256];

void RegisterSyscall(unsigned int ID, unsigned int Position)
{
	asm ("mov %0, %%ebx" : : "r"(ID));
	asm ("mov %0, %%edx" : : "r"(Position));
	asm ("xor %eax, %eax");
	asm ("int $0x80");
}

void SYS_RegisterSyscall(struct Registers32 Regs)
{
	Syscalls[Regs.EBX] = Regs.EDX;
}

void IntSyscall_Handler(struct Registers32 Regs)
{
	void (*F)(struct Registers32) = (void (*)(struct Registers32))Syscalls[Regs.EAX];
	// We only want to call the function if it is not null!
	if (F)
	{
		F(Regs);
	}
}

void Syscalls_Initialize(void)
{
	puts(
"Setup default syscalls:\n\
    0=SYS_RegisterSyscall\n\
    1=SYS_RegisterInterruptHandler\n", 0x0B);
	Syscalls[0] = (unsigned int)SYS_RegisterSyscall;
	Syscalls[1] = (unsigned int)SYS_RegisterInterruptHandler;
	for (int i = 2; i < 0xFF; i++)
	{
		Syscalls[i] = 0;
	}
}

