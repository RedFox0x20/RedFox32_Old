#ifndef KEYBOARD_H
#define KEYBOARD_H

/* -------------------------------- Scancodes ------------------------------- */
/* ------------------- Codes > 0x80 are (mostly) releases ------------------- */
/*  US QWERTY Assumed
    Scan code set 1
    0x01 - Escape
    0x02 - 1
    0x03 - 2
    0x04 - 3
    0x05 - 4
    0x06 - 5
    0x07 - 6
    0x08 - 7
    0x09 - 8
    0x0A - 9
    0x0B - 0
    0x0C - -
    0x0D - =
    0x0E - Backspace
    0x0F - Tab
    0x10 - Q
    0x11 - W
    0x12 - E
    0x13 - R
    0x14 - T
    0x15 - Y
    0x16 - U
    0x17 - I
    0x18 - O
    0x19 - P
    0x1A - [
    0x1B - ]
    0x1C - Enter
    0x1D - Left Control
    0x1E - A
    0x1F - S
    0x20 - D
    0x21 - F
    0x22 - G
    0x23 - H
    0x24 - J
    0x25 - K
    0x26 - L
    0x27 - ;
    0x28 - '
    0x29 - `
    0x2A - Left Shift
    0x2B - \
    0x2C - Z
    0x2D - X
    0x2E - C
    0x2F - V
    0x30 - B
    0x31 - N
    0x32 - M
    0x33 - ,
    0x34 - .
    0x35 - /
    0x36 - Right Shift
    0x37 - Keypad *
    0x38 - Left Alt
    0x39 - Space
    0x3A - CapsLock
    0x3B -> 0x44 - F1 -> F10
    0x45 - NumLock
    0x46 - ScrollLock
    0x47 - Keypad 7
    0x48 - Keypad 8
    0x49 - Keypad 9
    0x4A - Keypad -
    0x4B - Keypad 4
    0x4C - Keypad 5
    0x4D - Keypad 6
    0x4E - Keypad +
    0x4F - Keypad 1
    0x50 - Keypad 2
    0x51 - Keypad 3
    0x52 - Keypad 0
    0x53 - Keypad .
    0x54 -> 0x56 - None
    0x57 - F11
    0x58 - F12
*/

/* -------------------------------- Functions ------------------------------- */

enum KBD_KEYSTATE
{
    KBD_DOWN = 0,
    KBD_UP = 1
};

void KBD_Init();

void KBD_SetLEDS(
    unsigned char NumLock,
    unsigned char CapsLock,
    unsigned char ScrollLock);

void KBD_Enable(void);

#endif