#include <Kernel/TextMode.h>

static unsigned int ScreenPos = 0;

void TextMode_Setup(void)
{
	ScreenPos = 0;
}

void ScrollScreen(unsigned char Lines)
{
	unsigned char
		*WritePos = (unsigned char *)TEXT_MODE_VIDEO_MEMORY,
		*ReadPos = (unsigned char *)
			(TEXT_MODE_VIDEO_MEMORY + TEXT_MODE_XRESOLUTION * Lines);

	/* There is probably a better way to do this...
	*/
	while (
			ReadPos != 
			(unsigned char *)(TEXT_MODE_VIDEO_MEMORY + TEXT_MODE_MEMORY_SIZE)
		  )
	{
		*WritePos = *ReadPos;
		*ReadPos = 0;
		WritePos++;
		ReadPos++;
	}
}

void putch(char ch, char Colour)
{
	/* Ensure we stick within our bounds
	*/
	if (ScreenPos >= TEXT_MODE_MEMORY_SIZE)
	{
		ScrollScreen(1);
		ScreenPos -= TEXT_MODE_XRESOLUTION;
	}
	switch (ch)
	{
		case '\n':
			ScreenPos = (ScreenPos + TEXT_MODE_XRESOLUTION) -
				(ScreenPos % TEXT_MODE_XRESOLUTION);
			return;
			/* Make a TAB equivalent to 4 spaces */
		case '\t':
			putch(' ', Colour);
			putch(' ', Colour);
			putch(' ', Colour);
			putch(' ', Colour);
			return;
			/* For the sake of input \b should be handled by a program reading input
			 * in however we will also have it remove from the screen just for fun.
			 */
		case '\b':
			ScreenPos-=2;
			putch(' ', Colour);
			ScreenPos-=2;
			/* ScreenPos is unsigned so this works through magic.
			*/
			if (ScreenPos > ScreenPos+2)
			{
				ScreenPos = 0;
			}
			return;
		default:
			TEXT_MODE_VIDEO_MEMORY[ScreenPos++] = ch;
			TEXT_MODE_VIDEO_MEMORY[ScreenPos++] = Colour;
			break;
	}
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

void putch_binary(char c, char Colour)
{
	for (int i = 0; i < 8; i++)
	{
		putch((c << i) & 0b10000000 ? '1' : '0', 0x0C);
	}
}

void DEBUG_TextMode_ShowColours(void)
{
	puts("[TEXT MODE] Showing all colour combinations.\n", 0x0C);
	short c;
	for (c = 0; c < 0x100; c++)
	{
		putch('#', (char)c);
	}
	putch('\n', 0x0A);
}

void ClearScreen(void)
{
	ScreenPos = 0;
	for (unsigned int i = 0; i < TEXT_MODE_MEMORY_SIZE; i++)
	{
		TEXT_MODE_VIDEO_MEMORY[i] = 0;
	}
}
