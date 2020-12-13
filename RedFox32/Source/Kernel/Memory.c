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

/*******************************************************************************
 * Memory management
 */

#define ALLOCATION_HEADER_MAGIC ((unsigned int)0xA110C000)
struct MemoryMap *Memory_MMAP;
static void *NextAllocationPosition = MEMORY_START;

struct MemoryAllocation
{
	unsigned int AllocationHeader;
	struct MemoryAllocation *Previous, *Next;
	unsigned int Length;
	unsigned char Flags;
	char Data[];
} __attribute__((packed));

struct
{
	struct MemoryAllocation *First, *Last;
	unsigned int NumAllocs;
} MemoryAllocationList;

void free(void *Data)
{
	struct MemoryAllocation *Alloc = 
		(struct MemoryAllocation *)
		((char*)Data - sizeof(struct MemoryAllocation));

	if (Alloc->AllocationHeader == ALLOCATION_HEADER_MAGIC)
	{
		Alloc->Flags = 0;
		Alloc->AllocationHeader++;
	}
	else
	{
		puts("[MEMORY] Invalid free!\n", 0x0C);
	}
}

void *malloc(unsigned int);
static unsigned char CanSafelyAllocate(void **Position, unsigned int Size)
{
	if (Size == 0) { return 1; }
	/* Ensure our bounds won't wrap round below 0
	*/
	if (
			*Position < (void*)MEMORY_START 
			|| *Position > (void*)0xFFFF0000
	   )
	{
		return 0;
	}

	if (
			(void*)((char*)*Position+Size) < (void*)MEMORY_START
			|| (void*)((char*)*Position+Size) > (void*)0xFFFF0000
	   )
	{
		return 0;
	}

	for (int i = 0; i < Memory_MMAP->NumEntries; i++) 
	{
		struct MemoryMapEntry *Entry = Memory_MMAP->Entries+i;
		/* If the end of the entry is before our position then we don't care
		 * about it.
		 */
		if (
				((void*)(((char*)(unsigned int)Entry->Start)
					+ Entry->Length))
				< *Position)
		{
			continue;
		}


		/* Check if we end up in the unsafe region
		*/
		if (
				(*Position >= (void*)(unsigned int)Entry->Start && *Position <= 
				 (void*)((char*)((unsigned int)Entry->Start) + Entry->Length))
				||
				(((void*)((char*)*Position + Size) 
				  >= (void*)(unsigned int)Entry->Start)
				 && (void*)((char*)*Position + Size) <=
				 (void*)((char*)((unsigned int)Entry->Start) + Entry->Length))
		   )
		{
			malloc((unsigned int)Entry->Start - (unsigned int)*Position);
			i = -1;
		}
	}
	return 1;
}

void *malloc(unsigned int Size)
{
	struct MemoryAllocation *Alloc;
	if (Size == 0) { return (void*)0; }
	if (CanSafelyAllocate(&NextAllocationPosition, Size))
	{
		Alloc = NextAllocationPosition;
		if (NextAllocationPosition != MEMORY_START)
		{
			Alloc->Previous 		= MemoryAllocationList.Last;
			Alloc->Previous->Next 	= Alloc;
		}
		else
		{
			Alloc->Previous = (struct MemoryAllocation*)0;

			MemoryAllocationList.First = Alloc;
		}

		Alloc->AllocationHeader		= ALLOCATION_HEADER_MAGIC; 
		Alloc->Flags 				= 1;
		Alloc->Length 				= Size;
		MemoryAllocationList.Last 	= Alloc;
		MemoryAllocationList.NumAllocs++;

		NextAllocationPosition = 
			(void*)((char*)NextAllocationPosition
					+ Size 
					+ sizeof(struct MemoryAllocation)
					+ 1);	

		return Alloc->Data;
	}

	/* Temporary
	*/
	while (1)
	{
		puts("[MALLOC] OUT OF MEMORY!\n", 0x0C);
		asm volatile("cli"); asm volatile("hlt"); 
	};
	return (void*)0;
}

void *realloc(void *Mem, unsigned int Size)
{
	struct MemoryAllocation *Alloc = 
		(struct MemoryAllocation *)
		((char*)Mem - sizeof(struct MemoryAllocation));

	if (Alloc->AllocationHeader != ALLOCATION_HEADER_MAGIC) { return (void*)0; }
	if (Alloc->Length >= Size) { Alloc-> Length = Size; return Alloc->Data; }
	else
	{
		if (Alloc->Next == (struct MemoryAllocation*)0)
		{
			Alloc->Length = Size;
			return Alloc->Data;
		}
		else
		{
			void *NewDataAlloc = malloc(Size);
			if (NewDataAlloc == (void*)0)
			{
				return (void*)0;
			}
			free(Mem);
			return NewDataAlloc;
		}
	}
}



void MemoryManagement_Setup(struct MemoryMap *MMAP)
{
	Memory_MMAP = MMAP;
	NextAllocationPosition = MEMORY_START; 
}

void memset(void *Mem, char value, unsigned int Length)
{
	while (Length--)
	{
		*((char*)Mem++) = value;
	}
}
