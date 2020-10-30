#if !defined(BOOTLOADER_STAGE2_SYSCALL_H) && !defined(COMMON_SYSCALL_H)
#define COMMON_SYSCALL_H
#include <Common/FDCInterface.h>
#include <Common/RegisterDefinitions.h>

static void RegisterSyscall(unsigned int ID, void (*Func)(struct Registers32))
{
	asm ("mov %0, %%ebx" : : "r"(ID));
	asm ("mov %0, %%edx" : : "r"(Func));
	asm ("xor %eax, %eax");
	asm ("int $0x80");
}

static void SYS_GetFDCInterface(struct FDCInterface *Interface)
{
	asm volatile ("mov %0, %%edx" : : "r"(Interface));
	asm volatile ("mov $0x10, %eax");
	asm volatile ("int $0x80");
}

#endif
