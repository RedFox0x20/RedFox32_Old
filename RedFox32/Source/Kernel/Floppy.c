#include <Kernel/Floppy.h>
#include <Kernel/IDT.h>
#include <Kernel/IO.h>
#include <Kernel/TextMode.h>

enum FloppyRegisters
{
	REG_STATUS_A 	= 0x03F0,
	REG_STATUS_B 	= 0x03F1,
	REG_DOR			= 0x03F2,
	REG_TDR			= 0x03F3,
	REG_MSR			= 0x03F4,
	REG_DRSR		= 0x03F4,
	REG_FIFO		= 0x03F5,
	REG_DIR			= 0x03F7,
	REG_CCR			= 0x03F7
};

enum DORFlags
{
	DOR_DRIVE_A		= 0,
	DOR_DRIVE_B		= 1,
	DOR_DRIVE_C		= 2,
	DOR_DRIVE_D		= 3,
	DOR_RESET		= 4,
	DOR_IRQ			= 8,
	DOR_MOTOR_A		= 0x10,
	DOR_MOTOR_B		= 0x20,
	DOR_MOTOR_C		= 0x40,
	DOR_MOTOR_D		= 0x80
};

enum MSRFlags
{
	MSR_ACTIVITY_A	= 1,
	MSR_ACTIVITY_B	= 2,
	MSR_ACTIVITY_C	= 4,
	MSR_ACTIVITY_D	= 8,
	MSR_BUSY		= 0x10,
	MSR_NDMA		= 0x20,
	MSR_DIO			= 0x40,
	MSR_RQM			= 0x80
};

enum FloppyCommands
{
	CMD_READ_TRACK 	= 2,
	CMD_SPECIFY 	= 3,
	CMD_STATUS		= 4,
	CMD_WRITE		= 5,
	CMD_READ		= 6,
	CMD_RECALIBRATE	= 7,
	CMD_SENSE_INT	= 8,
	CMD_WRITE_DEL	= 9,
	CMD_READ_ID		= 10,
	CMD_READ_DEL	= 12,
	CMD_FORMAT		= 13,
	CMD_DUMP		= 14,
	CMD_SEEK		= 15,
	CMD_VERSION		= 16,
	CMD_SCAN_EQUAL	= 17,
	CMD_CONFIGUrE	= 19,
	CMD_LOCK		= 20,
	CMD_VERIFY		= 22,
};

enum FloppyOptions
{
	OPT_MT = 0x80,
	OPT_MF = 0x40,
	OPT_SK = 0x20
};

static volatile char IRQReceived;

/* Increment the IRQReceived counter
 */
void IRQHandler(void)
{
	IRQReceived++;
}

/* WaitIRQ
 * Waits until atleast 1 IRQ has been received.
 * This method helps to ensure that we don't get stuck waiting for an interrupt
 * that has already happened.
 */
static inline void WaitIRQ(void)
{
	while (IRQReceived <= 0)
	{
		asm volatile ("hlt");
	}
	IRQReceived--;
}

/* DMAInit
 * We need to setup DMA so that the FDC reads and writes directly to memory
 * without interrupts having to handle the data.
 */
static inline void DMAInit(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 		*/
	outb(0xD8, 0xFF); /* Reset flip-flop 	*/
	outb(0x04, 0x00); /* Address low 		*/
	outb(0x04, 0x7E); /* Address high		*/
	outb(0xD8, 0xFF); /* Reset flip-flop	*/
	outb(0x05, 0xFF); /* Count low			*/
	outb(0x05, 0x01); /* Count high 		*/
	outb(0x80, 0x00); /* Paging 			*/
	outb(0x0A, 0x02); /* Unmask DMA 2 		*/
}

/* DMARead
 * Set DMA to read mode
 */
static inline void DMARead(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 	*/
	outb(0x0B, 0x56); /* Read settings 	*/
	outb(0x0A, 0x02); /* Unmask DMA 2	*/
}

/* DMAWrite
 * Set DMA to write mode
 */
static inline void DMAWrite(void)
{
	outb(0x0A, 0x06); /* Mask DMA 2 	*/
	outb(0x0B, 0x5A); /* Write settings */
	outb(0x0A, 0x02); /* Unmask DMA 2	*/
}

/* Initialize floppy values
*/
void Floppy_Setup(void)
{
	IRQReceived = 0;
	DMAInit();
}
