#include <Include/VESA.h>
#include <Include/VESAFont.h>
#include <Include/Shared_syscall.h>

static struct VESA_ModeInfo
{
  unsigned short ModeAttributes;
  unsigned char WinAAttributes;
  unsigned char WinBAttributes;
  unsigned short WinGranularity;
  unsigned short WinSize;
  unsigned short WinASegment;
  unsigned short WinBSegment;
  unsigned long WinFuncPtr;
  unsigned short BytesPerScanLine;
  unsigned short XResolution;
  unsigned short YResolution;
  unsigned char XCharSize;
  unsigned char YCharSize;
  unsigned char NumberOfPlanes;
  unsigned char BitsPerPixel;
  unsigned char NumberOfBanks;
  unsigned char MemoryModel;
  unsigned char BankSize;
  unsigned char NumberOfImagePages;
  unsigned char Reserved_page;
  unsigned char RedMaskSize;
  unsigned char RedMaskPos;
  unsigned char GreenMaskSize;
  unsigned char GreenMaskPos;
  unsigned char BlueMaskSize;
  unsigned char BlueMaskPos;
  unsigned char ReservedMaskSize;
  unsigned char ReservedMaskPos;
  unsigned char DirectColor_VESA_Mode_Info;
  unsigned long PhysBasePtr;
  unsigned long OffScreenMemOffset;
  unsigned short OffScreenMemSize;
  unsigned char Reserved[206];
} __attribute__((packed)) *_VESA_Mode_Info = (struct VESA_ModeInfo *)(0x8600);

static struct VESA_VesaInfo
{
  char VESASignature[4];
  unsigned char VESAVersion[2];
  unsigned int OEMStringPtr;
  unsigned char Capabilities[4];
  unsigned short VideoModePtr[2];
  unsigned short TotalMemory;
  unsigned short OemSoftwareRev;
  unsigned int OemVendorNamePtr;
  unsigned int OemProductNamePtr;
  unsigned int OemProductRevPtr;
  char Reserved[222];
  char OemData[256];
} __attribute__((packed)) *_VESA_Info = (struct VESA_VesaInfo *)(0x8800);

typedef struct __attribute__((packed))
{
  unsigned short di, si, bp, sp, bx, dx, cx, ax;
  unsigned short gs, fs, es, ds, eflags;
} regs16_t;

void VESA_SetVideoMode(unsigned short Mode)
{
  regs16_t regs;

  regs.ax = 0x4F02;
  regs.bx = Mode; //0x011B
  syscall(SYSCALL_ID_INT32, 0x10, &regs);

  VESA_GetModeInfo(Mode);
}

void VESA_SetBank(int bankNo)
{
  regs16_t regs;

  regs.ax = 0x4f05;
  regs.bx = 0x0;
  regs.dx = bankNo;

  syscall(SYSCALL_ID_INT32, 0x10, &regs);
}

void VESA_Init()
{
  regs16_t regs;
  memset(_VESA_Info, 0, sizeof(_VESA_Info));
  _VESA_Info->VESASignature[0] = 'V';
  _VESA_Info->VESASignature[1] = 'B';
  _VESA_Info->VESASignature[2] = 'E';
  _VESA_Info->VESASignature[3] = '2';
  memset(&regs, 0, sizeof(regs16_t));

  regs.ax = 0x4F00;
  regs.di = (unsigned int)_VESA_Info & 0xF;
  regs.es = (((unsigned int)_VESA_Info) >> 4) & 0xFFFF;
  syscall(SYSCALL_ID_INT32, 0x10, &regs);

  // Assumption that it is possible
  VESA_SetVideoMode(0x011B);
  VESA_SetBank(0);

  char xresstr[10] = {0};
  char yresstr[10] = {0};
  itoa(_VESA_Mode_Info->XResolution, xresstr, 10);
  itoa(_VESA_Mode_Info->YResolution, yresstr, 10);
  KHI_Write("VESA Initialised: ");
  KHI_Write(xresstr);
  KHI_putch('x');
  KHI_Write(yresstr);
  KHI_putch('\n');
}

void VESA_GetModeInfo(unsigned short Mode)
{
  regs16_t regs;
  memset(&regs, 0, sizeof(regs16_t));

  regs.ax = 0x4F01;
  regs.cx = Mode;
  regs.es = (((unsigned int)_VESA_Mode_Info) >> 4) & 0xFFFF;
  regs.di = (unsigned int)_VESA_Mode_Info & 0xF;
  syscall(SYSCALL_ID_INT32, 0x10, &regs);
}

void VESA_DrawPixel(unsigned int X, unsigned int Y, unsigned char R, unsigned char G, unsigned char B)
{
  unsigned char *Where = (void *)_VESA_Mode_Info->PhysBasePtr + (X * 3) + Y * (_VESA_Mode_Info->XResolution * 3);
  *Where++ = B;
  *Where++ = G;
  *Where = R;
}

void VESA_DrawFillRect(unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned char R, unsigned char G, unsigned char B)
{
  unsigned char *Where = (void *)_VESA_Mode_Info->PhysBasePtr + (X * 3) + Y * (_VESA_Mode_Info->XResolution * 3);
  for (unsigned int i = 0; i < H; i++)
  {
    for (unsigned int j = 0; j < W; j++)
    {
      *Where++ = B;
      *Where++ = G;
      *Where++ = R;
    }
    Where += (_VESA_Mode_Info->XResolution * 3) - (W * 3);
  }
}

void VESA_DrawRect(unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned char R, unsigned char G, unsigned char B)
{
  unsigned char *Where = (void *)_VESA_Mode_Info->PhysBasePtr + (X * 3) + Y * (_VESA_Mode_Info->XResolution * 3);
  for (unsigned int i = 0; i < H; i++)
  {
    for (unsigned int j = 0; j < W; j++)
    {
      if (i == 0 || i == H - 1 || j == 0 || j == W - 1)
      {
        *Where++ = B;
        *Where++ = G;
        *Where++ = R;
      }
      else
      {
        *Where++ = 0;
        *Where++ = 0;
        *Where++ = 0;
      }
    }
    Where += (_VESA_Mode_Info->XResolution * 3) - (W * 3);
  }
}

void VESA_DrawChar(unsigned char c, unsigned int X, unsigned int Y, unsigned char R, unsigned char G, unsigned char B, unsigned int Scale)
{
  const char *FontBytes = VESAFont8x8[c];
  int mask[8] = {1, 2, 4, 8, 16, 32, 64, 128};
  for (unsigned char y = 0; y < 8 * Scale; y++)
  {
    for (unsigned char x = 0; x < 8 * Scale; x++)
    {
      if ((FontBytes[y / Scale] & mask[x / Scale]) != 0)
      {
        VESA_DrawPixel(X + x, Y + y, R, G, B);
      }
      else
      {
        VESA_DrawPixel(X + x, Y + y, 0, 0, 0);
      }
    }
  }
}

void VESA_DEBUG_STROBE()
{
  while (1)
  {
    unsigned int Pos = 1;
    for (unsigned short B = 0; B < 255; B++)
    {
      for (unsigned short G = 0; G < 255; G++)
      {
        for (unsigned short R = 0; R < 255; R++)
        {
          ((char *)_VESA_Mode_Info->PhysBasePtr)[Pos++] = (char)B;
          ((char *)_VESA_Mode_Info->PhysBasePtr)[Pos++] = (char)G;
          ((char *)_VESA_Mode_Info->PhysBasePtr)[Pos++] = (char)R;
          Pos %= _VESA_Mode_Info->XResolution * _VESA_Mode_Info->YResolution * 3;
        }
        Pos += _VESA_Mode_Info->XResolution * 3;
        Pos %= _VESA_Mode_Info->XResolution * _VESA_Mode_Info->YResolution * 3;
      }
      Pos += _VESA_Mode_Info->XResolution * 3;
      Pos %= _VESA_Mode_Info->XResolution * _VESA_Mode_Info->YResolution * 3;
    }
    for (unsigned int i = 0; i < 0xFFFFFF; i++)
      ;
  }
}