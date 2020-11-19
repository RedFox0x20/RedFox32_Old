#include <BootloaderStage2/Video.h>

static volatile unsigned int ScreenPos = 0;

void ScrollScreen(unsigned char Lines)
{
	unsigned char
		*WritePos = (unsigned char *)VIDEO_MEMORY,
		*ReadPos = (unsigned char *)(VIDEO_MEMORY + VGA_XRESOLUTION * Lines);

	/* There is probably a better way to do this...
	 */
	while (ReadPos != (unsigned char *)(VIDEO_MEMORY + VGA_MEMORY_SIZE))
	{
		*WritePos = *ReadPos;
		WritePos++;
		ReadPos++;
	}
}

void putch(char ch, char colour)
{
	// Ensure we stick within our bounds
	if (ScreenPos >= VGA_MEMORY_SIZE)
	{
		ScrollScreen(1);
		ScreenPos -= VGA_XRESOLUTION;
	}
	if (ch == '\n')
	{
		ScreenPos = (ScreenPos + VGA_XRESOLUTION) -
					(ScreenPos % VGA_XRESOLUTION);
		return;
	}
	VIDEO_MEMORY[ScreenPos++] = ch;
	VIDEO_MEMORY[ScreenPos++] = colour;
}

void puts(char *str, char colour)
{
	while (*str != 0)
	{
		putch(*str, colour);
		str++;
	}
}
