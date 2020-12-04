#include <Kernel/IO.h>
#include <Kernel/IDT.h>
#include <Kernel/TextMode.h>
#include <Kernel/Keyboard.h>
#include <Kernel/KeyboardLayouts/GB.h>

struct KeyboardModifiers Modifiers;
struct KeyboardEvent	 PreviousEvent;


void UpdateLEDS(void)
{
	char c;
	while (((c = inb(KEYBOARD_CTRL_PORT)) & 0x02) != 0) inb(KEYBOARD_DATA_PORT);
	outb(KEYBOARD_DATA_PORT, 0xED);
	while (((c = inb(KEYBOARD_CTRL_PORT)) & 0x02) != 0) inb(KEYBOARD_DATA_PORT);
	outb(KEYBOARD_DATA_PORT,*(unsigned char*)&Modifiers);
}

static void UpdateEventModifiers(void)
{
	PreviousEvent.Modifiers = Modifiers;
}

void Keyboard_Handler(void)
{
	unsigned char Keycode = inb(KEYBOARD_DATA_PORT);
	
	/* NOTE
	 * Some extra processing is needed for Extended keys such as Print Screen as
	 * these keys may proceed to have the scancode for other keys, in this
	 * example, print screen has the same keycode as an asterisk, 0x37 as per
	 * numpad *.
	 */
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
		case KEY_CONTROL:
			Modifiers.Control	= State;
			UpdateEventModifiers();
			return;
		
		case KEY_LEFT_SHIFT:
			Modifiers.LeftShift = State;
			UpdateEventModifiers();
			return;
		
		case KEY_RIGHT_SHIFT:
			Modifiers.RightShift = State;
			UpdateEventModifiers();
			return;
		
		case KEY_CAPS_LOCK:
			if (State == KEY_STATE_UP) { Modifiers.CapsLock ^= 1; }
			UpdateLEDS();
			UpdateEventModifiers();
			return;
		
		case KEY_CAPS_IGNORE:
			return;
		
		case KEY_NUM_LOCK:
			if (State == KEY_STATE_UP) { Modifiers.NumLock ^= 1; }
			UpdateLEDS();
			UpdateEventModifiers();
			return;
		
		case KEY_SCROLL_LOCK:
			if (State == KEY_STATE_UP) { Modifiers.ScrollLock ^= 1; }
			UpdateLEDS();
			UpdateEventModifiers();
			return;
		
		case KEY_ALT:
			Modifiers.Alt = State;
			UpdateEventModifiers();
			return;

		case KEY_MENU_LEFT:
			Modifiers.LeftMenu = State;
			UpdateEventModifiers();
			return;
		
		case KEY_MENU_RIGHT:
			Modifiers.RightMenu = State;
			UpdateEventModifiers();
			return;
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

	PreviousEvent = 
	(struct KeyboardEvent) {
		.Ready = 1,
		.Keycode = Keycode,
		.Character = c,
		.State = State,
		.Modifiers = Modifiers
	};
	if (Extended)
	{
		PreviousEvent.Keycode |= 0x0E00;
	}

#if 0
	/* This code is useful for finding out what certain keys are and under what
	 * conditions they fire. I.E certain modifiers.
	 */
	puts("[KEYBOARD]", 0x0A);
	puts(" Modifiers=", 0x0A);
	putch_hex(*((char*)&Modifiers+1), 0x0A);
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
#endif
}

void Keyboard_Setup(void)
{
	*((short*)&Modifiers) = 0;
	UpdateLEDS();
	SetInterruptHandler(HANDLER_KEYBOARD, Keyboard_Handler);
}

struct KeyboardEvent Keyboard_GetEvent(void)
{
	static struct KeyboardEvent Event;
	while (!PreviousEvent.Ready) asm volatile("hlt");
	for (int i = 0; i < sizeof(struct KeyboardEvent); i++)
	{
		((char*)&Event)[i] = ((char*)&PreviousEvent)[i];
		((char*)&PreviousEvent)[i] = 0;
	}
	return Event;
}

char Keyboard_getch(void)
{
	char c = 0;
	while (
			c == 0 
			|| PreviousEvent.State != KEY_STATE_DOWN 
			|| !PreviousEvent.Ready
		)
	{
		asm volatile ("hlt");
		c = PreviousEvent.Character;
		PreviousEvent.Ready = 0;
	}
	return c;
}
