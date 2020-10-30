#ifndef COMMON_FDCINTERFACE_H
#define COMMON_FDCINTERFACE_H

/* Provide a common reference to the FDCInterface structure.
 * This interface can be gathered using the SYS_GetFDCInterface syscall
 * EDX=Interface ptr
 * EAX=0x10
 * INT 0x80
 */

struct FDCInterface
{
	void
		(*ReadLBA)(unsigned short LBA),
		(*WriteLBA)(unsigned short LBA);
};

void inline GetFDCInterface(struct FDCInterface *Interface)
{
	asm volatile ("mov %0, %%edx" : : "r"(Interface));
	asm volatile ("mov $0x10, %eax");
	asm volatile ("int $0x80");
}

#endif
