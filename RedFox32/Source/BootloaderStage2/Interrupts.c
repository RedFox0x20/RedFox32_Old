#include <BootloaderStage2/Syscall.h>
#include <BootloaderStage2/Interrupts.h>
#include <BootloaderStage2/IO.h> 

/* struct IDT_Entry defines the structure of an IDT Entry
 * These entries follow a standard layout that will be used by the system.
 * Due to this factor the structure must be packed to ensure the compiler
 * doesn't add any padding bytes.
 * The structure is 8 bytes in size.
 */
struct IDT_Entry
{
	unsigned short
		low,
		selector;
	unsigned char
		zero,
		type_attr;
	unsigned short high;
} __attribute__((packed));

/* Reserve enough space for 256 IDT entries.
 * This structure is packed.
 */
static struct IDT_Entry IDT[256];

/* Reserve some space for us to store the memory address of the Interrupt
 * handler functions.
 */
static unsigned int InterruptHandlers[16];

/* Create a list of functions that will be used to register handlers for
 * interrupts.
 */
extern void
	Int0(void),
	Int1(void),
	Int2(void),
	Int3(void),
	Int4(void),
	Int5(void),
	Int6(void),
	Int7(void),
	Int8(void),
	Int9(void),
	Int10(void),
	Int11(void),
	Int12(void),
	Int13(void),
	Int14(void),
	Int15(void),
	IntSyscall(void);

/* Take the previous values and place them into an array of addresses
 */
static void (*InterruptCalls[])(void) =
{
	Int0,
	Int1,
	Int2,
	Int3,
	Int4,
	Int5,
	Int6,
	Int7,
	Int8,
	Int9,
	Int10,
	Int11,
	Int12,
	Int13,
	Int14,
	Int15
};
	
/* To load the interrupt table we need a function that has been defined in the
 * assembly file.
 */
extern int LoadIDT();

void RegisterIRQ(unsigned int InterruptID, unsigned long Address)
{
	IDT[InterruptID].low 		= Address & 0xFFFF;
	IDT[InterruptID].selector   = 0x08;
	IDT[InterruptID].zero		= 0;
	IDT[InterruptID].type_attr  = 0x8E;
	IDT[InterruptID].high		= (Address & 0xFFFF0000) >> 16;
}

/* Once a change has been made to the IDT it is important that we reload it.
 * To do this we get the position of the IDT and call to our assmebler function
 * LoadIDT((struct IDT_Entry)IDTPtr[256]);
 */
static void ReloadIDT(void)
{
	unsigned long IDTAddr = (unsigned long)IDT;
	unsigned long IDTPtr[2];
	IDTPtr[0] = (sizeof(struct IDT_Entry) * 256) + ((IDTAddr & 0xFFFF) << 16);
	IDTPtr[1] = IDTAddr >> 16;

	LoadIDT(IDTPtr);

}

/* Before interrupts are enabled we need to register our intterupt handlers and
 * remap the PIC so that it knows we are using our own interrupt handlers and
 * not the BIOS defaults.
 */
void IDT_Initialize(void)
{
	/* PIC remap, this is used to tell the PIC that we no longer want
	 * to use the BIOS default interrupt mappings.
	 */
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 40);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0);
	outb(0xA1, 0);

	/* We want to set the user defined interrupts to call to our stub functions
	 * which push their integer ID to the stack which can then be read and call
	 * to a user defined function as per the RegisterInterruptHandler function.
	 * User defined interrupts start from index 32 through 48(?)
	 */
	for (unsigned int i = 32; i < 48; i++)
	{
		RegisterIRQ(i, (unsigned long)InterruptCalls[i-32]);
		InterruptHandlers[i-32] = 0;
	}
	/* Int 0x80 is a special use case.
	 * 0x80 is used for software interrupts AKA Syscalls.
	 */
	RegisterIRQ(0x80, (unsigned long)IntSyscall);

	/* We have set the values we want to use now we have to load our IDT.
	 */
	ReloadIDT();
}


/* Define a function which can be used to register interrupt handlers.
 * This functions just calls into our syscall which is a bit unnecessary however
 * it does ensure that any changes to the structure used will only require one
 * set of code and a common call.
 */
void RegisterInterruptHandler(unsigned int ID, unsigned int Position)
{
	asm ("mov %0, %%ebx" : : "r"(ID));
	asm ("mov %0, %%edx" : : "r"(Position));
	asm ("mov $0x01, %eax");
	asm ("int $0x80");
	return;
}

/* Define a system call which is used to add an interrupt handler
 */
void SYS_RegisterInterruptHandler(struct Registers32 Regs)
{
	IDT_DisableInterrupts();
	InterruptHandlers[Regs.EBX] = Regs.EDX;
	IDT_EnableInterrupts();
}

/* We're defining a stub handler that allows for a common point for all of out
 * interrupts to call into to then be redirected to the user designated
 * interrupt handlers. By doing this I can control the data that is on the stack
 * easier, I.E parameter passing where necessary however it is atypical for
 * parameters to need to be passed to interrupt handlers as the data they are
 * handling typically exists in system hardware which is gathered from reading
 * from the system IO ports.
 */
void InterruptHandlerStub(int ID)
{
	void (*F)(void) = (void (*)(void))InterruptHandlers[ID];
	if (F != 0)
	{
		F();
	}
	if (ID >= 8)
	{
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
}

