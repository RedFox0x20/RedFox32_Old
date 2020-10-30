#ifndef BOOTLOADER_STAGE2_H
#define BOOTLOADER_STAGE2_H

#define FDC_READWRITE_BUFFER ((char*)0x7E00)

#include <Common/FDCInterface.h>

void FDC_Init(void);
void FDC_GetFDCI(struct FDCInterface *Interface);

#endif
