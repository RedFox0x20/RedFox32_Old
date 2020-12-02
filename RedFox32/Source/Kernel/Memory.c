#include <Kernel/Memory.h>
#include <Kernel/TextMode.h>

/* Return the number of bytes used by the memory map.
 * 0x8000->0x????
 * 192 (0xc0) * NumEntries + 4
 */
unsigned int MMAP_Length(struct MemoryMap *Map)
{
	return (sizeof(struct MemoryMapEntry) * Map->NumEntries) + 4;
}

void MMAP_Display(struct MemoryMap *MMAP)
{
	puts("[MMAP] Displaying Memory Map Entries\n", 0x0C);
	int MMAPEntries_Count = MMAP->NumEntries;
	puts("MMAP_Entries: 0x", 0x0B);

	putch_hex(((char*)&MMAPEntries_Count)[3], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[2], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[1], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[0], 0x0B);
	puts(
			"\nLOW         HIGH        LEN LOW     LEN HIGH    TYPE        ACPI\n",
			0x0B
		);

	char *b = (char*)MMAP->Entries;
	for (int i = 0; i < MMAPEntries_Count; i++)
	{
		/* Entries are 6*4 bytes long
		 * 2*8 byte entries
		 * 2*4 byte entries
		 */
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
