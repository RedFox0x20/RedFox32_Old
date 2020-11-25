OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(KMain)

SECTIONS
{
	.text : 
	{
		Build/Kernel/KEntry.o(*.text)
		*(.text)
	}
}
