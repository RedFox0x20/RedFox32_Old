#include <BootloaderStage2/FDC.h>
#include <BootloaderStage2/Interrupts.h>
#include <BootloaderStage2/Syscall.h>
#include <BootloaderStage2/IO.h>
#include <BootloaderStage2/Video.h>

/* ENUMS
*/

enum FloppyRegisters
{
	REG_STATUS_A = 0x03F0,
	REG_STATUS_B = 0x03F1,
	REG_DOR = 0x03F2,
	REG_TDR = 0x03F3,
	REG_MSR = 0x03F4,
	REG_DRSR = 0x03F4,
	REG_FIFO = 0x03F5,
	REG_DIR = 0x03F7,
	REG_CCR = 0x03F7
};

enum DORFlags
{
	DOR_DSEL0 = 0,
	DOR_DSEL1 = 1,
	DOR_DSEL2 = 2,
	DOR_DSEL3 = 3,
	DOR_RESETController = 4,
	DOR_IRQ = 8,
	DOR_MOTA = 0x10,
	DOR_MOTB = 0x20,
	DOR_MOTC = 0x40,
	DOR_MOTD = 0x80
};

enum MSRFlags
{
	MSR_ACTA = 1,
	MSR_ACTB = 2,
	MSR_ACTC = 4,
	MSR_ACTD = 8,
	MSR_BUSY = 0x10,
	MSR_NDMA = 0x20,
	MSR_DIO = 0x40,
	MSR_RQM = 0x80
};

enum FloppyCommands
{
	READ_TRACK = 2, /* generates IRQ6 */
	SPECIFY = 3,	/* set drive parameters */
	SENSE_DRIVE_STATUS = 4,
	WRITE_DATA = 5,		 /* write to the disk */
	READ_DATA = 6,		 /* read from the disk */
	RECALIBRATE = 7,	 /* seek to cylinder 0 */
	SENSE_INTERRUPT = 8, /* ack IRQ6, get status of last command */
	WRITE_DELETED_DATA = 9,
	READ_ID = 10, /* generates IRQ6 */
	READ_DELETED_DATA = 12,
	FORMAT_TRACK = 13, /* */
	DUMPREG = 14,
	SEEK = 15,	  /* seek both heads to cylinder X */
	VERSION = 16, /* used during initialization, once */
	SCAN_EQUAL = 17,
	PERPENDICULAR_MODE = 18, /* used during initialization, once, maybe */
	CONFIGURE = 19,			 /* set controller parameters */
	LOCK = 20,				 /* protect controller params from a resetController */
	VERIFY = 22,
	SCAN_LOW_OR_EQUAL = 25,
	SCAN_HIGH_OR_EQUAL = 29
};

enum FloppyCommandOptions
{
	OPT_MT = 0x80,
	OPT_MF = 0x40,
	OPT_SK = 0x20
};

/* Data
*/
static volatile unsigned char IRQReceived = 0; 
static unsigned char Drive = 0;

static void ResetController(void);

static void TimeoutPanic(void)
{
	puts("[FDC] PANIC: Timeout! \n", 0x0C);


	ResetController();
}

/* Interrupt handling
 * Interrupts are pretty basic in regards to the floppy driver as it is used to
 * inform the system of task completion.
 */
static void IRQHandler(void)
{
	puts("IRQ_REC\n", 0x0C);
	IRQReceived = 1;
}

static inline void WaitIRQ(void)
{
	puts("IRQ_WAIT\n", 0x0C);
	while (!IRQReceived)
	{
		asm("hlt");
	}
	IRQReceived = 0;
	puts("IRQ_REG\n", 0x0C);
}

/* Direct Memory Addressing (DMA)
 * Using DMA we are able to write straight into memory from the floppy drive.
 * In a monotasking environment this is a bit wasteful however, by starting with
 * interrupt mode over polling mode it will be simpler to allow thread safety
 * for multitasking environments in the future.
 */

static void DMAInit(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 */
	outb(0xD8, 0xFF); /* ResetController FlipFlop */
	outb(0x04, 0x00); /* Address Low bits  */
	outb(0x04, 0x7E); /* Address High bits 0x7E00 */
	outb(0xD8, 0xFF); /* ResetController FlipFlop */
	outb(0x05, 0xFF); /* Count low bits  */
	outb(0x05, 0x01); /* Count high bits 0x01FF (1K) 2 sectors */
	outb(0x80, 0x00); /* Page register 0 -> 0x00:0x7E00 */
	outb(0x0A, 0x02); /* Unmask DMA 2 */
}

static inline void DMARead(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 */
	/* Single transfer, address increment, auto init, read, channel 2 */
	outb(0x0B, 0x56);
	outb(0x0A, 0x02); /* Unmask DMA 2 */
}

static inline void DMAWrite(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 */
	/* Single transfer, address increment, auto init, write, channel 2 */
	outb(0x0B, 0x5A);
	outb(0x0A, 0x02); /* Unmask DMA 2 */
}

/* Register access
*/
static inline void WriteDOR(unsigned char Val)
{
	outb(REG_DOR, Val);
}

static inline unsigned char ReadMSR(void)
{
	return inb(REG_MSR);
}

static inline unsigned char ReadFIFO(void)
{
	while (!(ReadMSR() & MSR_RQM))
	{
		asm("nop");
	}
	return inb(REG_FIFO);
}

static inline void WriteFIFO(unsigned char Value)
{
	while (!(ReadMSR() & MSR_RQM))
	{
		asm("nop");
	}
	outb(REG_FIFO, Value);
}

static inline void WriteCCR(unsigned char Value)
{
	outb(REG_CCR, Value);
}

/* Commands
*/

static inline void MotorOn(void)
{
	WriteDOR(
			DOR_IRQ |
			DOR_RESETController | /* 0x0C  */
			(DOR_MOTA << Drive)); /* 0x10 << Drive */
}

static inline void MotorOff(void)
{
	WriteDOR(DOR_IRQ | DOR_RESETController); /* 0x0C */
}

static inline void SenseInterrupt(unsigned char *st0, unsigned char *cyl)
{
	WriteFIFO(SENSE_INTERRUPT);
	*st0 = ReadFIFO();
	*cyl = ReadFIFO();
}

static void Seek(unsigned char Cylinder, unsigned char Head)
{
	unsigned char st0, cyl;
	unsigned short Timeout = 0;
	do
	{
		WriteFIFO(SEEK);
		WriteFIFO((Head << 2) | Drive);
		WriteFIFO(Cylinder);

		WaitIRQ();
		SenseInterrupt(&st0, &cyl);

		if (Timeout++ == 500)
		{
			TimeoutPanic();
		}
	} while (cyl != Cylinder);
}

/* */

/* Write = 0 */
/* Read  = 1 */
static void ReadWriteGeneric(
		unsigned char ReadWrite,
		unsigned char Cylinder,
		unsigned char Head,
		unsigned char Sector)
{
	MotorOn();
	Seek(Cylinder, Head);


	unsigned char st0, cyl;
	ReadWrite ? DMARead() : DMAWrite();
	/* REMOVED THE SK FLAG
	 * TRY THIS ON HARDWARE!
	 */
	WriteFIFO((5 + ReadWrite) | OPT_MT | OPT_MF);
	/* The recomendation is to not set this bit however,
	 * the code example states otherwise...
	 */
	/*	OPT_SK);*/
	WriteFIFO((Head << 2) | Drive);
	WriteFIFO(Cylinder);
	WriteFIFO(Head);
	WriteFIFO(Sector);
	WriteFIFO(2);	 /* 512 BPS */
	WriteFIFO(18);	 /* 18 SPC */
	WriteFIFO(0x1B); /* GAP1 */
	WriteFIFO(0xFF); /* 512 BPS */

	WaitIRQ();
	for (int i = 0; i < 7; i++)
	{
		ReadFIFO();
	}

	SenseInterrupt(&st0, &cyl);

	MotorOff();
}

static inline void LBA2CHS(
		unsigned int LBA,
		unsigned char *Cylinder,
		unsigned char *Head,
		unsigned char *Sector)
{
	*Cylinder = LBA / 36;
	*Head = ((LBA % 36) / 18);
	*Sector = ((LBA % 36) % 18 + 1);
}

static void ReadLBA(unsigned short LBA)
{
	if (LBA >= 2880)
	{
		return;
	}

	unsigned char C, H, S;
	LBA2CHS(LBA, &C, &H, &S);
	ReadWriteGeneric(1, C, H, S);
}

static void WriteLBA(unsigned short LBA)
{
	if (LBA >= 2880)
	{
		return;
	}

	unsigned char C, H, S;
	LBA2CHS(LBA, &C, &H, &S);
	ReadWriteGeneric(0, C, H, S);
}

/* */

static inline void Specify(
		unsigned char StepRateTime,
		unsigned char HeadLoadTime,
		unsigned char HeadUnloadTime)
{
	WriteFIFO(SPECIFY);
	WriteFIFO((StepRateTime << 4) | HeadUnloadTime);
	WriteFIFO((HeadLoadTime << 1) | 1);
}

static void Calibrate(void)
{
	unsigned char st0, cyl;
	unsigned short Timeout = 0;
	MotorOn();
	do
	{
		WriteFIFO(RECALIBRATE);
		WriteFIFO(Drive);

		WaitIRQ();

		SenseInterrupt(&st0, &cyl);
		if (Timeout++ == 500)
		{
			TimeoutPanic();
		}
	} while (cyl);
	MotorOff();
}

static inline void FDC_Disable(void)
{
	WriteDOR(0);
}

static inline void FDC_Enable(void)
{
	WriteDOR(DOR_RESETController | DOR_IRQ);
}

static void ResetController(void)
{
	unsigned char st0, cyl;
	FDC_Disable();
	FDC_Enable();
	WaitIRQ();

	for (int i = 0; i < 4; i++)
	{
		SenseInterrupt(&st0, &cyl);
		ReadFIFO();
		ReadFIFO();
	}
	WriteCCR(0);
	Specify(13, 1, 0xF);
	
	/* Remove calibrate as a temporary potential fix
	 * Calibrate();
	 */
}

static void SYS_GetFDCI(struct Registers32 Regs)
{
	struct FDCInterface *FDCI = (struct FDCInterface *)Regs.EDX;
	FDCI->ReadLBA = ReadLBA;
	FDCI->WriteLBA = WriteLBA;
}

void FDC_Init(void)
{
	Drive = 0;
	IRQReceived = 0;
	RegisterInterruptHandler(6, (unsigned int)IRQHandler);
	RegisterSyscall(0x10, (unsigned int)SYS_GetFDCI);
	DMAInit();
	WriteFIFO(0x94);
	ReadFIFO();
	/* I'm having to trust the system to not fail it's specify on me */
}
