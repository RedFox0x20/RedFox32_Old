#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

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

#endif
