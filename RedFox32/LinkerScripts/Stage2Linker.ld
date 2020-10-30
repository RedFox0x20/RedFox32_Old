OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x0500;
    .text : {
        Build/BootloaderStage2/Stage2A.o(.text)
    }
	.data : {
		Build/BootloaderStage2/Stage2A.o(.data)
	}
	.text : {
		*(.text)
	}
	.data : {
		*(.data)
	}
}
