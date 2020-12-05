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

	/* Access the individual bytes to convert to hex
	 */
	putch_hex(((char*)&MMAPEntries_Count)[3], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[2], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[1], 0x0B);
	putch_hex(((char*)&MMAPEntries_Count)[0], 0x0B);

	/* Display headings to identify what each 4 byte section is for
	 */
	puts(
			"\nLOW         HIGH        LEN LOW     LEN HIGH    TYPE        ACPI\n",
			0x0B
		);

	/* Loop through the MMAP displaying it on the screen split into 32bit (4
	 * byte) sections.
	 */
	char *b = (char*)&MMAP->Entries;
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

struct MemoryAllocation
{
	struct MemoryAllocation *Previous, *Next;
	unsigned int Length;
	unsigned char Flags;
	/* Not all allocations are linear or in order so we just point to the data
	 * instead.
	 */
	void *Data;
};

struct
{
	struct MemoryAllocation *First, *Last;
	unsigned int Count;
} AllocationList;

struct MemoryAllocation _List_First = 
{
	.Previous 	= (struct MemoryAllocation*)0,
	.Next		= (struct MemoryAllocation*)0,
	/* This value may need changing, just prevent things from being allocated
	 * within the kernel memory space and video memory. This is also a standard
	 * value.
	 * */
	.Length		= 0x00100000,
	.Flags = ALLOCATION_USED,
	.Data = (void*)0;
}

void AllocationList_Add(void *Data, unsigned int Count)
{
	struct MemoryAllocation *Alloc = 
		((unsigned char*)AllocationList.Last) 
		+ sizeof(struct MemoryAllocation)
		+ AllocationList.Last.Length;

	*Alloc = 
		(struct MemoryAllocation)
		{
			.Previous = AllocationList.Last,
			.Next = (struct MemoryAllocation *)0,
			.Length = Count,
			.Flags = ALLOCATION_USED,
			.Data = Data
		};
	AllocationList.Last.Next = Alloc;
	AllocationList.Last = Alloc;
	AllocationList.Count++;
}

void MemoryManagement_Setup(struct MemoryMap *MMAP)
{
	AllocationList.First = &_List_First;
	AllocationList.Last  = &_List_First;
	AllocationList.Count = 1;

	for (unsigned int i = 0; i < MMAP->NumEntries; i++)
	{
		/* Check unusable regions */
		if (
				(MMPA->Entries[i].Type == MMAP_TYPE_UNUSABLE
				 || MMAP->Entries[i].Type == MMAP_TYPE_NVS)
				/* Our standard blocked region
				*/
				&& MMAP->Entries[i].Start > 0x00100000
		   )
		{
			/* Add the region to the list
			*/
			AllocationList_Add(MMAP->Entries[i].Start, MMAP->Entries[i].Lenght);
		}

	}
}
