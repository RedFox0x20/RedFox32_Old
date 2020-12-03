#include <Kernel/IDT.h>
#include <Kernel/Syscalls.h>
#include <Kernel/Memory.h>
#include <Kernel/TextMode.h>
#include <Kernel/Keyboard.h>

/* KMain
 * The primary C function for the kernel, all setup methods should be called
 * from here in the required order. When we enter this code our bootloader has
 * completed it's processes and called to 0x2900 which is where we have loaded
 * the kernel. This then runs some stub code (KEntry.asm) which furthers control
 * into the C code here. The stub is only necessary due to the linking process.
 */
int KMain(struct MemoryMap *MMAP)
{
	/* Initialize the video driver
	 */
	TextMode_Setup();
	TextMode_ShowColours();

	/* Core setup
	 */
	IDT_Setup();
	Syscalls_Setup();
	Keyboard_Setup();

	/* Once all drivers are setup we can enable interrupts.
	 */
	EnableInterrupts();

	/* Any debug methods can be called here safely.
	 */
	MMAP_Display(MMAP);	

	/* Forever call the hlt instruction.
	 * This instruction halts the CPU, this prevents it from doing anything
	 * until an interupt occours. Having the halt here prevents the CPU from
	 * sitting on a loop spending CPU cycles doing nothing but running at
	 * maximum speed for periods of time. This prevents the CPU from becoming a
	 * good frying pan. :)
	 *
	 * Halt allows for interrupts to occour, any hardware interrupts will pull
	 * the system back into the loop. This can be useful in cases where, for
	 * example we are waiting on either hardware or the user to perform an
	 * action.
	 */
	puts("Entered C halt loop!\n", 0x0C);
	for(;;)
	{
		asm volatile("hlt");
		struct KeyboardEvent Event = Keyboard_GetEvent();
		if (Event.Keycode != 0 && Event.State == KEY_STATE_UP)
		{
			puts("Received key: ", 0x0B);
			putch(Event.Character, 0x0B);
			putch('\n', 0x0B);
		}
	}
	
	/* Make it obvious for development purposes.
	 */
	puts("Kernel exiting! Returning to Bootloader for system stop.", 0x0C);
	return 0;
}
