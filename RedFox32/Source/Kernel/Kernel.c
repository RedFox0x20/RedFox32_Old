#include <Kernel/IDT.h>
#include <Kernel/Syscalls.h>
#include <Kernel/Memory.h>
#include <Kernel/TextMode.h>

void DEBUG_DumpMMAP(void)
{
	int MMAPEntries_Count = *((int*)0x8000);
	puts("MMAP_Entries: 0x", 0x0B);

	putch_hex(*(char*)0x8003, 0x0B);
	putch_hex(*(char*)0x8002, 0x0B);
	putch_hex(*(char*)0x8001, 0x0B);
	putch_hex(*(char*)0x8000, 0x0B);
	puts("\nLOW         HIGH        LEN LOW     LEN HIGH    TYPE        ACPI\n", 0x0B);
	char *b = (char*)0x8004;
	for (int i = 0; i < MMAPEntries_Count; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			puts("0x", 0x0B);
			for (int n = 3; n >= 0; n--)
			{
				char c = *(b+n);
				putch_hex(c, 0x0B);
			}
			b += 4;
			puts("  ", 0x0B);
		}
		putch('\n', 0x0B);
	}
}

/* KMain
 * The primary C function for the kernel, all setup methods should be called
 * from here in the required order. When we enter this code our bootloader has
 * completed it's processes and called to 0x2900 which is where we have loaded
 * the kernel. This then runs some stub code (KEntry.asm) which furthers control
 * into the C code here. The stub is only necessary due to the linking process.
 */
int KMain(struct MemoryMap *MMAP)
{
	IDT_Setup();
	Syscalls_Setup();
	EnableInterrupts();

	Video_Setup();
	DEBUG_DumpMMAP();	
	/* Forever call the hlt instruction.
	 * This instruction halts the CPU, this prevents it from doing anything
	 * until an interupt occours. Having the halt here prevents the CPU from
	 * sitting on a loop spending CPU cycles doing nothing but running at
	 * maximum speed for periods of time. This prevents the CPU from becoming a
	 * good frying pan. :)
	 */
	//for(;;) asm volatile("hlt");
	
	puts("Kernel exiting! Returning to Bootloader for halt.", 0x0C);
	return 0;
}
