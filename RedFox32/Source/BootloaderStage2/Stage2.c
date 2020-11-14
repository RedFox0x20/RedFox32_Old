// Stage 2 C, awesome!
// We are in a known video mode, 80 colums x 25 rows

#include <BootloaderStage2/Video.h>

#define KERNEL_POSITION ((void*)0x2200)
#define STAGE1_MEMORY_POSTION ((char*)0x7C00)

int Stage2_CEntry(void)
{
	while (1) puts("Entered C-Code!\n", VGA_COLOUR_GREEN);
	return 0;
}
