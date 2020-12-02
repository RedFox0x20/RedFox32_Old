#include <Kernel/IO.h>
#include <Kernel/IDT.h>
#include <Kernel/TextMode.h>
#include <Kernel/Keyboard.h>
#include <Kernel/KeyboardLayouts/GB.h>

/* KerboardModifiers
 * It is important to keep the lock keys in this order as this is how they are
 * arranged for setting the keyboard LEDs.
 */
struct KeyboardModifiers
{
	unsigned char
		ScrollLock	:1, /* Bit 0 */
		NumLock		:1, /* Bit 1 */
		CapsLock	:1, /* Bit 2 */
		LeftShift	:1, /* Bit 4 */
		RightShift	:1; /* Bit 5 */
} Modifiers;

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

	/* Perform modifier actions
	 */
	switch (Keycode)
	{
		case 0x2A:
			Modifiers.LeftShift = State;
			return;
		case 0x36:
			Modifiers.RightShift = State;
			return;
		case 0x3A:
			if (State == KEY_STATE_UP) { Modifiers.CapsLock ^= 1; }
			return;
		default:
			break;
	}

	/* For regular keys we should be using an upper case character when either
	 * shift or caps lock is enabled. If both capslock and shift then remain
	 * lower.
	 * In most real world applications, caps lock doesn't do symbols however for
	 * simplicity I am going to leave it to also convert symbols.
	 */
	char c = 
		(Modifiers.LeftShift || Modifiers.RightShift) ^ Modifiers.CapsLock ?
		  KeyboardLayoutCaps[Keycode]
		: KeyboardLayout[Keycode];

	puts("[KEYBOARD]", 0x0A);
	puts(" Modifiers=", 0x0A);
	putch_hex(*((char*)&Modifiers), 0x0A);
	puts(" Extended=0x", 0x0A);
	putch_hex(Extended, 0x0A);
	puts(" Keycode=0x", 0x0A);
	putch_hex(Keycode, 0x0A);
	puts(" State=0x", 0x0A);
	putch_hex(State, 0x0A);
	puts(" Char='", 0x0A);
	putch(c, 0x0A);
	puts("'\n", 0x0A);
}

void Keyboard_Setup(void)
{
	for (int i = 0; i < sizeof(struct KeyboardModifiers); i++)
	{
		((char*)&Modifiers)[i] = 0;
	}
	SetInterruptHandler(HANDLER_KEYBOARD, Keyboard_Handler);
}


