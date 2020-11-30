#include <Kernel/Memory.h>

/* Return the number of bytes used by the memory map.
 * 0x8000->0x????
 * 192 (0xc0) * NumEntries + 4
 */
unsigned int MMAP_Length(struct MemoryMap *Map)
{
	return (sizeof(struct MemoryMapEntry) * Map->NumEntries) + 4;
}

