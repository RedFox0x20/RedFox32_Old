OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)

SECTIONS
{
	. = 0x2900;
	.text : 
	{
		Build/Kernel/KEntry.o(*.text)
	}
	.data :
	{
		Build/Kernel/KEntry.o(*.data)
	}
	.text :
	{
		*(*.text)
	}
	.data :
	{
		*(*.data)
	}
}
