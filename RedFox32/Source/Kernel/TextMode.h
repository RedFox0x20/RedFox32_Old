#ifndef KERNEL_TEXT_MODE_H
#define KERNEL_TEXT_MODE_H

#define VIDEO_MEMORY ((char*)0xB8000)
#define VGA_XRESOLUTION 160
#define VGA_YRESOLUTION 25
#define VGA_MEMORY_SIZE VGA_XRESOLUTION * VGA_YRESOLUTION


void Video_Setup(void);
void putch(char c, char Colour);
void putch_hex(char c, char Colour);
void puts(char *str, char Colour);

#endif
