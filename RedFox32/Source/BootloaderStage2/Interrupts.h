#ifndef BOOTLOADER_STAGE2_INTERRUPTS_H
#define BOOTLOADER_STAGE2_INTERRUPTS_H

#include <BootloaderStage2/RegisterDefinitions.h>
#define IDT_DisableInterrupts() asm volatile ("cli")
#define IDT_EnableInterrupts() asm volatile ("sti")

void IDT_Initialize(void);
void RegisterInterruptHandler(unsigned int ID, unsigned int Position);
void SYS_RegisterInterruptHandler(struct Registers32 Regs);

#endif
