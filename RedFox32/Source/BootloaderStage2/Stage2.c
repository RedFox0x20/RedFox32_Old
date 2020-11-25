// Stage 2 C, awesome!
// We are in a known video mode, 80 colums x 25 rows

#include <BootloaderStage2/Video.h>

#define KERNEL_POSITION ((void*)0x2200)
#define STAGE1_MEMORY_POSTION ((char*)0x7C00)

void putch_hex(char c, char Colour)
{
	char h = c >> 4;
	h &= 0x0F;
	h |= 0x30;
	if (h > '9') { h += 7; }

	char l = c;
	l &= 0x0F;
	l |= 0x30;
	if (l > '9') { l += 7; }

	putch(h, Colour);
	putch(l, Colour);
}

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

int Stage2_CEntry(void)
{
	SetupVideo();
	DEBUG_DumpMMAP();
	return 0;
}
