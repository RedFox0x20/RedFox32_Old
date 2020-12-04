#ifndef KERNEL_TEXT_MODE_H
#define KERNEL_TEXT_MODE_H

#define TEXT_MODE_VIDEO_MEMORY ((char*)0xB8000)
#define TEXT_MODE_XRESOLUTION 160
#define TEXT_MODE_YRESOLUTION 25
#define TEXT_MODE_MEMORY_SIZE TEXT_MODE_XRESOLUTION * TEXT_MODE_YRESOLUTION

#define COLOUR_MASK_FG 0x0F
#define COLOUR_MASK_BG 0xF0

void TextMode_Setup(void);
void DEBUG_TextMode_ShowColours(void);

void putch(char c, char Colour);
void putch_hex(char c, char Colour);
void putch_binary(char c, char Colour);

void puts(char *str, char Colour);

void ClearScreen(void);

#endif
