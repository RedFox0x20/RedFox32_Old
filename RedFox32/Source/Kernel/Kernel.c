#include <Kernel/VGA.h>
#include <Common/Syscalls.h>
#include <Common/FDCInterface.h>
#include <Common/Conversion.h>
#include <Kernel/FileSystem.h>
#include <Kernel/Int32.h>
#include <Kernel/Memory.h>

static struct FDCInterface FDCI;
static struct FSInterface FSI;

/* Regs32.EAX = Interrupt
 * Regs32.EDI = regs16_t ptr
 */
void SYS_int32(struct Registers32 Regs32)
{
	int32(Regs32.EAX & 0xFF, (regs16_t *)Regs32.EDI);
}

void KMain()
{
	ClearScreen();
	puts("Kernel entered!\n", 0x0A);

	puts("Getting FDCInterface!\n", 0x0A);
	SYS_GetFDCInterface(&FDCI);

	puts("Init FS\n", 0x0A);
	FS_Initialize();

	puts("Getting FS Interface\n", 0x0A);
	asm("mov %0, %%edx"
		:
		: "r"(&FSI));
	asm("mov $0x20, %eax");
	asm("int $0x80");

	puts("Registering int32 syscall 0x03\n", 0x0A);
	RegisterSyscall(0x03, SYS_int32);

	puts("Attempting to read the test file!", 0x0B);
	FILE *F = (FILE *)malloc(sizeof(FILE));
	FSI.ReadFile(F, "/SYSTM");
	puts(F->Descriptor.Name, 0x0B);
	puts(F->Data, 0x0C);	
	
	char test[10];
	itoa(test, 54321);
	puts(test, 0x0D);
	/* *Load a FileSystem ~ Almost there
	 * *Load the drivers -- Use a dedicated file to identify drivers which will
	 *  then be auto-loaded and initialized -- We are assuming that the device
	 *  the driver is for will cause an interrupt. -- Take a look at PCI(e) and
	 *  examine how PCI(e) works as to how well this will work.
	 * * Load into a shell of sorts, it's all on the user at that point
	 *
	 *
	 *
	 */
}
