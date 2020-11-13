#include <Common/FDCInterface.h>
#include <Common/Syscalls.h>
#include <Common/RegisterDefinitions.h>
#include <Kernel/FileSystem.h>
#include <Kernel/Memory.h>
#include <Kernel/VGA.h>

/* Assumes floppy
 * Assumes 512 sector sizes
 */

struct FSRoot
{
	unsigned char
		Jump[3],
		DiskName[8];
	unsigned short
		FSRootDirectoryLBA,
		SectorMapLBA;
	unsigned char
		BootDisk,
		BootloaderCode[494];
	unsigned short
		BootMagicBytes;
} __attribute__((packed));

struct FSSectorMap
{
	unsigned char SectorGroups[360];
	unsigned short
		UsedSectors,
		FreeSectors;
} __attribute__((packed));

struct FSDirectoryDescriptor
{
	struct FSFileDescriptor Files[51];
	unsigned short NextDescriptorLBA;
} __attribute__((packed));

static const struct FSRoot *Root = (struct FSRoot *)0x7C00;
static struct FSSectorMap SectorMap;
static struct FSDirectoryDescriptor RootDirectory;
static struct FDCInterface FDCI;

static void LoadSectorMap(void)
{
	FDCI.ReadLBA(Root->SectorMapLBA);
	for (unsigned int i = 0; i < sizeof(struct FSSectorMap); i++)
	{
		((char *)&SectorMap)[i] = ((char *)0x7E00)[i];
	}
}

static void LoadFSRootDirectory(void)
{
	FDCI.ReadLBA(Root->FSRootDirectoryLBA);
	for (unsigned int i = 0; i < 512; i++)
	{
		((char *)&RootDirectory)[i] = ((char *)0x7E00)[i];
	}
}

static unsigned char GetSectorStatus(unsigned short Sector)
{
	unsigned int SectorGroupID = Sector / 8;
	unsigned int SectorBitOffset = Sector % 8;
	unsigned char SectorGroup = SectorMap.SectorGroups[SectorGroupID];
	unsigned char SectorBit = 0b10000000 >> SectorBitOffset;
	unsigned char SectorStatus = (SectorGroup & SectorBit) >>
								 (7 - SectorBitOffset);
	return SectorStatus;
}

static void SetSectorStatus(unsigned short Sector, unsigned char Status)
{
	Status = Status ? 1 : 0;
	unsigned int SectorGroupID = Sector / 8;
	unsigned int SectorBitOffset = Sector % 8;
	unsigned char SectorGroup = SectorMap.SectorGroups[SectorGroupID];
	unsigned char SectorBit = 0b10000000 >> SectorBitOffset;
	unsigned char SectorStatus = (SectorGroup & SectorBit) >>
								 (7 - SectorBitOffset);
	if (SectorStatus != Status)
	{
		SectorGroup ^= (1 << (7 - SectorBitOffset));
		SectorMap.SectorGroups[SectorGroupID] = SectorGroup;
	}
}

static unsigned int NameCompare(char A[5], char B[5])
{
	for (unsigned int i = 0; i < 5; i++)
	{
		if (A[i] != B[i])
		{
			return 0;
		}
	}
	return 1;
}

static void WriteSectorMapChanges(void)
{
	FDCI.ReadLBA(Root->SectorMapLBA);
	for (unsigned int i = 0; i < sizeof(struct FSSectorMap); i++)
	{
		((char *)0x7E00)[i] = ((char *)&SectorMap)[i];
	}
	FDCI.WriteLBA(Root->SectorMapLBA);
}

static void ReadDirectory(struct FSDirectoryDescriptor *Dir, char Name[5])
{
	for (unsigned int i = 0; i < 51; i++)
	{
		if (Dir->Files[i].Attributes.Directory)
		{
			if (NameCompare(Dir->Files[i].Name, Name))
			{
				FDCI.ReadLBA(Dir->NextDescriptorLBA);
				for (unsigned int c = 0; c < 512; i++)
				{
					((char *)Dir)[c] = ((char *)0x7E00)[c];
				}
				return;
			}
		}
	}
	if (Dir->NextDescriptorLBA != 0)
	{
		FDCI.ReadLBA(Dir->NextDescriptorLBA);
		for (unsigned int c = 0; c < 512; c++)
		{
			((char *)Dir)[c] = ((char *)0x7E00)[c];
		}
		ReadDirectory(Dir, Name);
		return;
	}
	Dir = (struct FSDirectoryDescriptor *)0;
}

static void ReadFile(FILE *F, char *Path)
{
	struct FSDirectoryDescriptor Dir = RootDirectory;
	unsigned char InDir;
	char *DirnameStart = Path, *DirnameEnd = Path;
	while (*DirnameStart == '/')
	{
		DirnameStart++;
	}
	while (!InDir)
	{
		while (*DirnameEnd != '/')
		{
			if (*DirnameEnd == 0)
			{
				InDir = 1;
				break;
			}
			DirnameEnd++;
		}
		if (InDir)
		{
			break;
		}
		*DirnameEnd = 0;
		ReadDirectory(&Dir, DirnameStart);
		DirnameEnd++;
		DirnameStart = DirnameEnd;
	}
	
	for (unsigned int i = 0; i < 51; i++)
	{
		if (NameCompare(Dir.Files[i].Name, DirnameStart))
		{
			puts("Match!\n", 0x0B);
			for (unsigned int j = 0; j < sizeof(struct FSFileDescriptor); j++)
			{
				((char *)&F->Descriptor)[j] = ((char *)&Dir.Files[i])[j];
			}
			F->Size = F->Descriptor.SectorCount * 512;
			F->Data = (char *)malloc(F->Size);

			for (
				unsigned short LBA = F->Descriptor.StartSector;
				LBA < F->Descriptor.StartSector + F->Descriptor.SectorCount;
				LBA++)
			{
				FDCI.ReadLBA(LBA);
				int Pos = 0;
				for (unsigned int c = 0; c < 512; c++)
				{
					F->Data[Pos++] = ((char *)0x7E00)[c];
				}
			}
			return;
		}
	}
}

static void SYS_GetFSInterface(struct Registers32 Regs)
{
	/* Some interface Read FDC.c */
	struct FSInterface *Interface = (struct FSInterface *)Regs.EDX;
	Interface->ReadFile = ReadFile;
}

void FS_Initialize(void)
{
	asm("mov %0, %%edx"
		:
		: "r"(&FDCI));
	asm("mov $0x10, %eax");
	asm("int $0x80");
	LoadFSRootDirectory();
	LoadSectorMap();
	RegisterSyscall(0x20, SYS_GetFSInterface);
}
