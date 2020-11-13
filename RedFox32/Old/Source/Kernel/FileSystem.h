#ifndef SOURCE_KERNEL_FILESYSTEM_H
#define SOURCE_KERNEL_FILESYSTEM_H


struct FSFileDescriptor
{
	char Name[5];
	struct
	{
		unsigned char
			Read 		:1,
						Write 		:1,
						Execute 	:1,
						Immutable 	:1,
						Hidden 		:1, 
						Directory	:1;
	} __attribute__((packed)) Attributes;
	unsigned short
		StartSector,
		SectorCount;
} __attribute__((packed));

typedef struct FILE
{
	struct FSFileDescriptor Descriptor;
	char *Data;
	unsigned int Size;
} __attribute__((packed)) FILE;

struct FSInterface
{
	void (*ReadFile)(FILE *F, char *Path);
};

void FS_Initialize(void);

#endif
