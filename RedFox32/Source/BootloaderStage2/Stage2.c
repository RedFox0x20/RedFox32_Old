// Stage 2 C, awesome!
// We are in a known video mode, 80 colums x 25 rows

#include <BootloaderStage2/Stage2.h>
#include <BootloaderStage2/Video.h>

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
	return 0;
}
