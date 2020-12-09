#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#define MMAP_TYPE_USABLE 1
#define MMAP_TYPE_UNUSABLE 2
#define MMAP_TYPE_RECLAIMABLE 3
#define MMAP_TYPE_NVS 4

#define MEMORY_PAGE_SIZE (1024 * 4)
#define MEMORY_NUM_PAGES ((0xFFFF0000 - 0x00100000) / MEMORY_PAGE_SIZE)
#define MEMORY_NUM_PAGE_BYTES (MEMORY_NUM_PAGES / 8)
#define MEMORY_START ((void*)0x00100000)


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

void MemoryManagement_Setup(struct MemoryMap *MMAP);
void *malloc(unsigned int size);
void *realloc(void *Mem, unsigned int Size);
void free(void *Mem);
#endif
