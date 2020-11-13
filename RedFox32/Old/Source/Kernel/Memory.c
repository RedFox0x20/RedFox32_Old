/* Memory.c should provide functionality to allow for dynamic memory allocation
 * within designated ranges.
 *
 * This is a very simplistic memory allocator.
 * It is definitely not safe, as every block of memory can just casually access
 * the next by overruning by 15 bytes (More bytes than the 15 can/will lead to
 * further issues..
 *
 * Memory (re/de)allocation	[malloc] [realloc] [calloc] [free]
 */

#define MALLOC_INITIAL_POSITION 0x20004

static const char *ALLOCATION_MAGIC = "A110C";

/* Define a list of Unsafe regions where we cannot allocate memory.
 *
 * In regards to unsafe regions, it would be reasonable to call allocated
 * regions as unsafe. To avoid writing into existing alloctions a simple check
 * to see if the next allocation region is lower in memory than the current last
 * allocation, if this is the case then it is assumable that we have reached
 * the end of the memory and will have to attempt to allocate within free space.
*/
static struct MemoryRegion
{
	void *Start, *End;
} UnsafeRegions[] = {
	/* Reserve the first 64K of memory to the Kernel.
	 * This is likely more memory than I will need for now however these values
	 * can be adjusted.
	 */
	{.Start = (void *)0, .End = (void *)0x10000},
	/* Kernel */
	{.Start = (void *)0x10000, .End = (void *)0x20000},
	/* ISA Memory Hole, only a loss of 1MB / Reserved
	*/
	{.Start = (void *)0x00100000, .End = (void *)0x00EFFFFF},
	/* This is memory that should be mapped however as a temporary solution it
	 * will just be completely blocked off.
	 */
	{.Start = (void *)0xC0000000, .End = (void *)0xFFFFFFFF}};

#define NUM_UNSAFE_REGIONS \
	((unsigned int)(sizeof(struct MemoryRegion) / sizeof(UnsafeRegions)))

/* Allocation header, holds the size of an allocation, the allocations which
 * surround it and its usage state.
 * Size: 15 bytes.
 */
struct AllocationHeader
{
	char AllocationMagic[5]; /* A110C */
	unsigned int AllocationSize;
	struct AllocationHeader *Next, *Previous;
	unsigned char Free;
} __attribute__((packed));

static struct
{
	struct AllocationHeader *First, *Last;
} __attribute__((packed)) MemoryAllocationList =
	{
		.First = (struct AllocationHeader *)0,
		.Last = (struct AllocationHeader *)0};

static void *NormalAllocation(unsigned int Size)
{
	/* Find the location of the previous last allocation
	*/
	struct AllocationHeader *LastNode = MemoryAllocationList.Last;
	/* Calculate the position of the new node location
	*/
	struct AllocationHeader *NewAllocation =
		(struct AllocationHeader *)((char *)LastNode +
									LastNode->AllocationSize +
									sizeof(struct AllocationHeader));

	if (LastNode == (struct AllocationHeader *)0)
	{
		/* A bit of a hack...*/
		NewAllocation = (struct AllocationHeader *)MALLOC_INITIAL_POSITION;
	}

	/* Ensure that the new allocation position is not within one of the unsafe
	 * zones
	 */

	if ((unsigned int)NewAllocation < (unsigned int)LastNode)
	{
		// Memory wrap we need to try and allocate in free space
		// This will be checked for in the malloc function for ease.
		return (void *)0;
	}

	for (unsigned int i = 0; i < NUM_UNSAFE_REGIONS; i++)
	{
		if (
			// Check we don't start in an unsafe region
			(UnsafeRegions[i].Start <= (void *)NewAllocation &&
			 UnsafeRegions[i].End >= (void *)NewAllocation) ||
			// Check we don't end in an unsafe region
			(UnsafeRegions[i].Start <= (void *)((char *)NewAllocation + Size) &&
			 UnsafeRegions[i].End >= (void *)((char *)NewAllocation + Size)))
		{
			/* It may be worth allocating this region anyway and assigning it
			 * as free.
			 *
			 * Set the New allocation position to be clear of the unsafe region
			 * and repeat.
			 */
			NewAllocation =
				(struct AllocationHeader *)((char *)UnsafeRegions[i].End + 1);

			/* Check that we haven't reverted to memory behind us...
			*/
			if ((unsigned int)NewAllocation < (unsigned int)LastNode)
			{
				// Memory wrap we need to try and allocate in free space
				// This will be checked for in the malloc function for ease.
				return (void *)0;
			}
		}
	}

	/* Make the previous LastNode aware that the new node exists.
	*/
	if (LastNode != (struct AllocationHeader *)0)
	{
		LastNode->Next = NewAllocation;
	}
	else
	{
		MemoryAllocationList.First = NewAllocation;
		MemoryAllocationList.Last = NewAllocation;
	}

	/* Setup the new node
	*/
	/* Could be looped into the ALLOCATION_MAGIC str however it's not really
	 * necessary, probably also faster to do this as it's a total of 9 mov
	 * commands
	 */
	NewAllocation->AllocationMagic[0] = 'A';
	NewAllocation->AllocationMagic[1] = '1';
	NewAllocation->AllocationMagic[2] = '1';
	NewAllocation->AllocationMagic[3] = '0';
	NewAllocation->AllocationMagic[4] = 'C';
	NewAllocation->AllocationSize = Size;
	NewAllocation->Next = (struct AllocationHeader *)0;
	NewAllocation->Previous = LastNode;

	/* The new allocation is now the last allocated block
	*/
	MemoryAllocationList.Last = NewAllocation;

	/* Return the allocated region (The region proceeding the header)
	*/
	return (void *)(((char *)NewAllocation) + sizeof(struct AllocationHeader));
}

static void JoinFreeBlocks(void)
{
	struct AllocationHeader *Alloc = MemoryAllocationList.First;
	while (Alloc != (struct AllocationHeader *)0)
	{
		/* If this node is free then we want to attempt a merge when possible
		*/
		if (Alloc->Free)
		{
			/* Merge with every free node after the current node.
			 * Total their memory into this node to create one large free node.
			 */
			while (Alloc->Next->Free)
			{
				struct AllocationHeader *FreeNode = Alloc->Next;
				Alloc->Next = Alloc->Next->Next;
				Alloc->Next->Previous = Alloc;
				Alloc->AllocationSize +=
					FreeNode->AllocationSize + sizeof(struct AllocationHeader);
			}
		}
		Alloc = Alloc->Next;
	}
}

static void *AllocateInFreeBlock(unsigned int Size)
{
	struct AllocationHeader *Alloc = MemoryAllocationList.First;
	/* Iterate over every allocated node until we find a node that is free and
	 * of a suitable size.
	 *
	 * Repeat until both conditions are correct, hence an OR
	 */
	while (!Alloc->Free || Alloc->AllocationSize < Size)
	{
		Alloc = Alloc->Next;
		/* We have checked EVERY possible location... There is no more memory
		 * available to dynamically allocate into... Congrats?
		 */
		if (Alloc == (struct AllocationHeader *)0)
		{
			return (void *)0;
		}
	}

	/* We have found a suitable location to allocate into.
	 * For now we are just going to perform a greedy allocation and allocate the 
	 * entire region to the allocation.
	 * It may be valuable in the future to only take what we need here.
	 */
	Alloc->Free = 0;
	return (void *)((char *)Alloc + sizeof(struct AllocationHeader));
}

void *malloc(unsigned int Size)
{
	void *Ret = NormalAllocation(Size);
	if (Ret != (void *)0)
	{
		return Ret;
	}
	JoinFreeBlocks();
	Ret = AllocateInFreeBlock(Size);
	if (Ret == (void *)0)
	{
		/* There is no more memory available... What will we do...
		 * PANIC!!!!!!!!!!!!!
		 */
		for (;;)
		{
			asm("cli");
			asm("hlt");
		}
	}
	return Ret;
}

/* Allocates Count of Size elements
*/
void *calloc(unsigned int Count, unsigned int Size)
{
	/* Allocate the memory block
	*/
	void *Ret = malloc(Count * Size);

	/* Initialize the block to zeroes
	*/
	for (unsigned int i = 0; i < Count * Size; i++)
	{
		/* Conversion to char* as I want to be performing, per byte operations
		*/
		((char *)Ret)[i] = 0;
	}

	return Ret;
}

void free(void *Memory)
{
	/* The given region is 99% an allocation header assuming it passes the
	  * magic test... A simple magic test isn't great however it is better than
	  * just handling anything and everything that gets thrown at us.
	 */
	struct AllocationHeader *Alloc =
		(struct AllocationHeader *)((char *)Memory - sizeof(struct AllocationHeader));
	/* Check magic
	 */
	for (int i = 0; i < 5; i++)
	{
		/* Check valid
		 */
		if (Alloc->AllocationMagic[i] != ALLOCATION_MAGIC[i])
		{
			/* Invalid
			 */
			return;
		}
	}
	/* Set the Free flag to Free
	*/
	Alloc->Free = 1;
}
