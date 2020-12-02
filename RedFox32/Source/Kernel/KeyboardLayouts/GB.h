#ifndef KERNEL_KBD_LAYOUT_H
#define KERNEL_KBD_LAYOUT_H

const char KeyboardLayout[256] = 
{
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	0, 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
	0, 0, '#', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
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