#ifndef KERNEL_IDT
#define KERNEL_IDT

#define HANDLER_PIT 0
#define HANDLER_KEYBOARD 1
/* Here for completeness, this is never raised. */
#define HANDLER_CASCADE 2
#define HANDLER_COM2 3
#define HANDLER_COM1 4
#define HANDLER_LPT2 5
#define HANDLER_FLOPPY 6
/* LPT1 is considered to be potentially spurious and unreliable */
#define HANDLER_LPT1 7
#define HANDLER_CMOS 8
#define HANDLER_PS2_MOUSE 12
#define HANDLER_FPU 13
#define HANDLER_COPROCESSOR 13
#define HANDLER_INTER_PROCESSOR 13
#define HANDLER_ATA_PRIMARY 14
#define HANDLER_ATA_SECONDARY 15
#define HANDLER_SYSCALLS 0x80

void IDT_Setup(void);
void SetInterruptHandler(unsigned char ID, void (*Func)(void));

#endif
