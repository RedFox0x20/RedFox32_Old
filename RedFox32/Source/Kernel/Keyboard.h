#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

#define KEYBOARD_DATA_PORT 0x60
#define KEY_STATE_UP 0
#define KEY_STATE_DOWN 1

void Keyboard_Setup(void);

#endif
