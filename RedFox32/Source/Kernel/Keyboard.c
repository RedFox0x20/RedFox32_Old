#include <Kernel/IO.h>
#include <Kernel/IDT.h>
#include <Kernel/TextMode.h>
#include <Kernel/Keyboard.h>
#include <Kernel/KeyboardLayouts/GB.h>

void Keyboard_Handler(void)
{
	unsigned char Keycode = inb(KEYBOARD_DATA_PORT);
	unsigned char Extended = (Keycode == 0xE0) ? 1 : 0;
	if (Extended)
	{
		Keycode = inb(KEYBOARD_DATA_PORT);	
	}
	unsigned char State = Keycode & 0x80 ? KEY_STATE_UP : KEY_STATE_DOWN;
	/* If they key is up then bit 0x80 is set so we can remove it to get the
	 * normal scancode
	 */
	if (State == KEY_STATE_UP)
	{
		Keycode -= 0x80;
	}

	puts("[KEYBOARD ", 0x0A);
	puts("Extended=0x", 0x0A);
	putch_hex(Extended, 0x0A);
	puts(" Keycode=0x", 0x0A);
	putch_hex(Keycode, 0x0A);
	puts(" State=0x", 0x0A);
	putch_hex(State, 0x0A);
	puts(" Char=", 0x0A);
	putch(KeyboardLayout[Keycode], 0x0A);
	puts("]\n", 0x0A);
}

void Keyboard_Setup(void)
{
	SetInterruptHandler(HANDLER_KEYBOARD, Keyboard_Handler);
}


