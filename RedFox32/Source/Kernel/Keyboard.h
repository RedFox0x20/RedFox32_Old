#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CTRL_PORT 0x64

#define KEY_STATE_UP 0
#define KEY_STATE_DOWN 1
#define MODIFIER_DISABLED 0
#define MODIFIER_ENABLED 1

/* KerboardModifiers
 * It is important to keep the lock keys in this order as this is how they are
 * arranged for setting the keyboard LEDs.
 */
struct KeyboardModifiers
{
	unsigned short 
		ScrollLock		:1, /* Bit 0  */
		NumLock			:1, /* Bit 1  */
		CapsLock		:1, /* Bit 2  */
		LeftShift		:1, /* Bit 4  */
		RightShift		:1, /* Bit 5  */
		Control 		:1, /* Bit 6  */
		Alt				:1, /* Bit 7  */
		LeftMenu		:1, /* Bit 8  */
		RightMenu		:1; /* Bit 9 */
};

/* struct KeyboardEvent
 * Keycode: The scancode of the key, some scancodes are altered for ease I.E
 * 			multi-byte scancodes
 * Character: The ASCII representation of the key. Non alphanumerics should have
 * 			  a 0 value
 * State: Is it a key down (1)? or a key up (0)?
 * Modifiers: The keyboard modifiers as presented by the KeyboardModifiers
 * 			  struct.
 */
struct KeyboardEvent
{
	unsigned short 	Keycode;
	unsigned char	Character;
	unsigned char	State;
	struct KeyboardModifiers Modifiers;
};

void Keyboard_Setup(void);
struct KeyboardEvent Keyboard_GetEvent(void);

#endif
