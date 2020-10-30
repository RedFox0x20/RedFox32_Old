/* Clone of BootloaderStage2/Video.c
 * This clone is used simply to give a basic method of text output to the screen
 * until a VESA driver has been added
 */
#include <Kernel/VGA.h>
static unsigned int ScreenPos = 0;

inline void ClearScreen(void)
{
	for (unsigned int i = 0; i < VGA_MEMORY_SIZE; i++)
	{
		VIDEO_MEMORY[i] = 0;
	}
}

void ScrollScreen(unsigned char Lines)
{
	unsigned char
		*WritePos = (unsigned char *)VIDEO_MEMORY,
		*ReadPos = (unsigned char *)(VIDEO_MEMORY + VGA_XRESOLUTION * Lines);

	while (ReadPos != (unsigned char *)(VIDEO_MEMORY + VGA_MEMORY_SIZE))
	{
		*WritePos = *ReadPos;
		WritePos++;
		ReadPos++;
	}
}

void putch(char ch, char Colour)
{
	// Ensure we stick within our bounds
	if (ScreenPos >= VGA_MEMORY_SIZE)
	{
		/* Clear the VRAM
		 * This should probably be replaced with a scrolling effect at some
		 * point, for now just clear to black.
		 */
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
	VIDEO_MEMORY[ScreenPos++] = Colour;
}

void puts(char *str, char colour)
{
	while (*str != 0)
	{
		putch(*str, colour);
		str++;
	}
}
