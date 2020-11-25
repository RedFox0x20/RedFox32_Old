#ifndef BOOTLOADER_STAGE2_VIDEO_H
#define BOOTLOADER_STAGE2_VIDEO_H

#define VIDEO_MEMORY ((char*)0xB8000)
/* Number of columns * 2, [Char][Colour][...]
 * We are using 80x25 16 colour text mode
 * [Char8][[BG4][FG4]]
 */
#define VGA_XRESOLUTION 160
#define VGA_YRESOLUTION 25
#define VGA_MEMORY_SIZE VGA_XRESOLUTION * VGA_YRESOLUTION


void SetupVideo(void);
void ScrollScreen(unsigned char Lines);
void putch(char ch, char Colour);
void puts(char *str, char Colour);

#endif
