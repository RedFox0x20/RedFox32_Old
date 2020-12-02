#include <Kernel/TextMode.h>

static unsigned int ScreenPos = 0;

void TextMode_Setup(void)
{
	ScreenPos = 0;
}

/* This function can be optimised... See `rep stosw`
 */
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

/* It could also be possible to improve the performance of puts with `rep movsb`
 * given that appropriate testing is performed to ensure we don't overrun the
 * memory available to us.
 */
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

void TextMode_ShowColours(void)
{
	puts("[TEXT MODE] Showing all colour combinations.\n", 0x0C);
	short c = 0;
	for (; c < 0x100; c++)
	{
		putch('#', (char)c);
	}
	putch('\n', 0x0A);
}
