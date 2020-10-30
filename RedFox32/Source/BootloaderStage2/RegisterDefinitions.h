#ifndef BOOTLOADER_STAGE2_REGISTER_DEFINITIONS_H
#define BOOTLOADER_STAGE2_REGISTER_DEFINITIONS_H

struct Registers32
{
	unsigned int EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
};

struct Registers16
{
	unsigned short DI, SI, BP, SP, BX, DX, CX, AX;
};

#endif
