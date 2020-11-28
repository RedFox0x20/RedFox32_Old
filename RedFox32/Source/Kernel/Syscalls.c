#include <Kernel/IDT.h>
#include <Kernel/Registers32.h>

void SyscallHandler(struct Registers32 Regs)
{
	/* Here will be a switch case to select commands.
	 * We could borrow from the BIOS and use ah for group and al for the command
	 * action, providing arguments in ebx, ecx, and edx, or in cases where n
	 * arguments are required, provide the number of args in ecx and a pointer
	 * to the arguments in edx/esi/edi.
	 *
	 * Until this is required I will just leave the function blank.
	 */
	
	return;
}

/* Syscalls_Setup
 * Initialise any syscall values.
 * Inform the IDT of our SyscallHandler function.
 */
void Syscalls_Setup(void)
{
	/* Set our handler in the IDT handlers list
	 */
	SetInterruptHandler(HANDLER_SYSCALLS, (void (*)(void))SyscallHandler);
}
