OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)

SECTIONS
{
	. = 0x10000;
	.text :
	{
		Build/Kernel/KernelA.o(.text)
	}
	.data : {
		Build/Kernel/KernelA.o(.data)
	}
	.text : {
		*(.text)
	}
	.data : {
		*(.data)
	}
}

