OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)


SECTIONS
{
	. = 0x2900;
	.text :
	{
		Build/Kernel/KEntry.o(*.text)
		Build/Kernel/Kernel*.o(*.text)
		*.o(*.text)
	}
	.data :
	{
		*.o(*.data)
	}
	.bss :
	{
		*.o(*.bss)
	}
}
