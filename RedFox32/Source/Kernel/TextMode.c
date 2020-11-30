#include <Kernel/TextMode.h>

static unsigned int ScreenPos = 0;

void Video_Setup(void)
{
	ScreenPos = 0;
}

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

void putch(char ch, char Colour)
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
	VIDEO_MEMORY[ScreenPos++] = Colour;
}

void puts(char *str, char Colour)
{
	while (*str != 0)
	{
		putch(*str, Colour);
		str++;
	}
}

void putch_hex(char c, char Colour)
{
	char h = c >> 4;
	h &= 0x0F;
	h |= 0x30;
	if (h > '9') { h += 7; }

	char l = c;
	l &= 0x0F;
	l |= 0x30;
	if (l > '9') { l += 7; }

	putch(h, Colour);
	putch(l, Colour);
}
