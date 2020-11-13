// #include <Drivers/VGA/VGA.h>

// void VGA_Init(unsigned char Mode)
// {
//     VGA_Data.Mode = Mode;
//     regs16_t regs;
//     regs.ax = VGA_Data.Mode;
//     int32(0x10, &regs);

//     VGA_Data.W = 640;
//     VGA_Data.H = 480;
//     VGA_Data.ColourDepth = 1;
// }

// int VGA_PutPixel(unsigned int X, unsigned int Y)
// {
//     if (X > VGA_Data.W || Y > VGA_Data.H) { return 1; }
//     register unsigned int Pos = X + (Y * VGA_Data.W);
//     VIDEO_MEMORY[Pos] = VGA_Data.Colour;
//     return 0;
// }