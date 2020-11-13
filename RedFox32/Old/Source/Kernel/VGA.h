/* Clone of BootloaderStage2/Video.c
 * This clone is used simply to give a basic method of text output to the screen
 * until a VESA driver has been added
 */

#ifndef KERNEL_TEMP_VGA_H
#define KERNEL_TEMP_VGA_H

#define VIDEO_MEMORY ((char*)0xB8000)
#define VGA_COLOUR_GREEN (char)0x0A
/* Number of columns * 2, [Char][Colour][...]
 * We are using 80x25 16 colour text mode
 * [Char8][[BG4][FG4]]
 */
#define VGA_XRESOLUTION 160
#define VGA_YRESOLUTION 25
#define VGA_MEMORY_SIZE VGA_XRESOLUTION * VGA_YRESOLUTION


void ClearScreen(void);
void ScrollScreen(unsigned char Lines);
void putch(char ch, char Colour);
void puts(char *str, char Colour);

#endif
