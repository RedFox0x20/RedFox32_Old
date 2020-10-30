#ifndef VESA_H
#define VESA_H

void VESA_Init();
//0x011B
// https://en.wikipedia.org/w/index.php?title=VESA_BIOS_Extensions&action=edit&section=14
void VESA_SetVideoMode(unsigned short Mode);
void VESA_GetModeInfo(unsigned short Mode);
void VESA_SetBank(int bankNo);

void VESA_DrawPixel(unsigned int X, unsigned int Y, unsigned char R, unsigned char G, unsigned char B);
void VESA_DrawFillRect(unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned char R, unsigned char G, unsigned char B);
void VESA_DrawRect(unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned char R, unsigned char G, unsigned char B);
void VESA_DrawChar(unsigned char c, unsigned int X, unsigned int Y, unsigned char R, unsigned char G, unsigned char B, unsigned int Scale);
void VESA_DEBUG_STROBE();

#endif