/* Keyboard layout
 * ALL keyboard layout definitions MUST include each of the following and be
 * modified to suit other layouts. Once a layout has been included other layouts
 * should be ignored (They MUST share a header guard).
 */

#ifndef KERNEL_KBD_LAYOUT_H
#define KERNEL_KBD_LAYOUT_H

#define KEY_CONTROL 0x1D
#define KEY_LEFT_SHIFT 0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_ALT 0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_NUM_LOCK 0x45
#define KEY_SCROLL_LOCK 0x46
#define KEY_CAPS_IGNORE 0x7A /* Caps lock seems to produce this too */

const char KeyboardLayout[256] = 
{
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
	0, '`', 0, '#', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 
	[0x47] = '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
	[0x56] = '\\'	
};

const char KeyboardLayoutCaps[256] = 
{
	/*	There should be a £ here but it doesn't exist in ASCII. 
	 *				VVV
	 */ 
	0, 0, '!', '"',  0, '$', '%', '^', '&', '*', '(', '0', '_', '+',
	0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    0, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@',
	0, 0, '~', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	/* Numpad, Special, Space */
	'*', 0, ' ', 
	/* Numpad */
	[0x47] = '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
	[0x56] = '|'	
};

#endif
