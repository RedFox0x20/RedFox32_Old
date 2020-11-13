// Stage 2 C, awesome!
// We are in a known video mode, 80 colums x 25 rows

#include <BootloaderStage2/Stage2.h>
#include <BootloaderStage2/Interrupts.h>
#include <BootloaderStage2/Syscall.h>
#include <BootloaderStage2/Video.h>
#include <BootloaderStage2/IO.h>
#include <BootloaderStage2/FDC.h>

#define STAGE1_MEMORY_POSTION ((char*)0x7C00)
#define KERNEL_MEMORY_POSITION ((char*)0x10000)
#define KERNEL_START_SECTOR ((unsigned int)18)
#define KERNEL_END_SECTOR ((unsigned int)30)

int Stage2_CEntry(void)
{
	/* We have entered the C-Code. From this point we need to finish
	 * initializing the system and preparing it for use. To do this we need to
	 * setup Interrupts and System Calls. This will also include loading the
	 * Kernel in which we will use the Floppy Driver to do so. Doing this
	 * somewhat limits our potential boot devices however it is possibly to
	 * bypass this limitation using the BIOS functions by performing all our
	 * loading in the pre 32 bit Assembly code.
	 */
	puts("Entered C-Code!\n", VGA_COLOUR_GREEN);

	/* We will firstly setup interrupts.
	 */
	puts("Setting up interrupts!\n", VGA_COLOUR_GREEN);
	IDT_DisableInterrupts();
	IDT_Initialize();
	IDT_EnableInterrupts();
	puts("Complete\n", VGA_COLOUR_GREEN);

	/* Syscalls are dependant on the IDT being setup correctly and are also
	 * required to be setup before any drivers as syscalls will be used for
	 * interfacing with drivers.
	 */
	puts("Initializing syscalls!\n", VGA_COLOUR_GREEN);
	Syscalls_Initialize();

	/* We are linking Stage 2 with the Floppy Driver. This is required to setup
	 * the FDC using the FDC_Init function. Read/Write is still done through the
	 * FDCInterface structure as we are not exposing Floppy Driver functions to
	 * the userspace.
	 */
	puts("Initializing Floppy Driver!\n", 0x0A);
	FDC_Init();

	puts("Getting FDCInterface!\n", 0x0A);
	struct FDCInterface FDCI;
	asm ("mov %0, %%edx" : : "r"(&FDCI));
	asm ("mov $0x10, %eax");
	asm ("int $0x80");
	// If the above fails we will very...VERY... quickly know about it :)
	/* A test call to the ReadLBA function. This is required simply to prevent
	 * booting any further if we can't actually do anything.
	 */
#if 0
	puts("Performing read test\n", 0x0B);
	FDCI.ReadLBA(0);
	if (*STAGE1_MEMORY_POSTION != *FDC_READWRITE_BUFFER)
	{
		puts("Read failed!\n", 0x0C);
		return 1;
	}
#endif

	puts("Load kernel\n", 0x0A);
	int offset = 0;
	for (
		unsigned short Sector = KERNEL_START_SECTOR;
		Sector < KERNEL_END_SECTOR;
		Sector++)
	{
		putch('R', 0x0B);
		FDCI.ReadLBA(Sector);
		for (int i = 0; i < 512; i++)
		{
			KERNEL_MEMORY_POSITION[offset++] = FDC_READWRITE_BUFFER[i];
		}
	}
	((void (*)(void))KERNEL_MEMORY_POSITION)();
	return 0;
}
