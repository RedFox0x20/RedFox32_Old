#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#define MMAP_TYPE_USABLE 1
#define MMAP_TYPE_UNUSABLE 2
#define MMAP_TYPE_RECLAIMABLE 3
#define MMAP_TYPE_NVS 4

#define ALLOCATION_FREE 0
#define ALLOCATION_USED 1

struct MemoryMapEntry
{
	unsigned long long 
		Start,
		Length;

	unsigned int
		Type,
		ACPI;
};

struct MemoryMap
{
	unsigned int NumEntries;
	struct MemoryMapEntry *Entries;
};

unsigned int MMAP_Length(struct MemoryMap *Map);
void MMAP_Display(struct MemoryMap *MMAP);

#endif
