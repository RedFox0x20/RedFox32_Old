#ifndef BOOTLOADER_STAGE2_SYSCALL_H
#define BOOTLOADER_STAGE2_SYSCALL_H

#include <BootloaderStage2/RegisterDefinitions.h>

void RegisterSyscall(unsigned int ID, unsigned int Position);
void IntSyscall_Handler(void);
void Syscalls_Initialize(void);

#endif
