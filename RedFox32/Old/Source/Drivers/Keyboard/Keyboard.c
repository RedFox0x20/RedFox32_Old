#include <Include/Keyboard.h>
#include <Include/KBDUS.h>
#include <Include/KBDUK.h>
#include <Include/IO.h>
// #include <Kernel/KHI.h>

/* -------------------------------- Registers ------------------------------- */

enum KBD_ENCODER_IO
{
    KBD_ENC_INPUT_BUFFER = 0x60,
    KBD_ENC_CMD_REGISTER = 0x60
};

enum KBD_CTRL_IO
{
    KBD_CTRL_STATS_REGISTER = 0x64,
    KBD_CTRLE_CMD_REGISTER = 0x64
};

/* ------------------------------ Status masks ------------------------------ */

enum KYBRD_CTRL_STATS_MASK
{

    KYBRD_CTRL_STATS_OUT_BUF = 1,    //00000001
    KYBRD_CTRL_STATS_IN_BUF = 2,     //00000010
    KYBRD_CTRL_STATS_SYSTEM = 4,     //00000100
    KYBRD_CTRL_STATS_CMD_DATA = 8,   //00001000
    KYBRD_CTRL_STATS_LOCKED = 0x10,  //00010000
    KYBRD_CTRL_STATS_AUX_BUF = 0x20, //00100000
    KYBRD_CTRL_STATS_TIMEOUT = 0x40, //01000000
    KYBRD_CTRL_STATS_PARITY = 0x80   //10000000
};

/* -------------------------------- Commands -------------------------------- */

enum KBD_ENC_COMMANDS
{
    KBD_ENC_CMD_SET_LEDS = 0xED,
    KBD_ENC_CMD_ECHO = 0xEE,
    KBD_ENC_CMD_SET_SCANCODE = 0xF0,
    KBD_ENC_CMD_GET_KBDID = 0xF2,
    KBD_ENC_CMD_SET_REPEAT = 0xF3,
    KBD_ENC_CMD_ENABLE = 0xF4,
    KBD_ENC_CMD_RESET_WAIT_FOR_ENABLE = 0xF5,
    KBD_ENC_CMD_RESET_SCAN_IMMEDIATE = 0xF6,
    KBD_ENC_CMD_AUTOREPEAT = 0xF7,
    KBD_ENC_CMD_ALL_SEND_MAKE_BREAK_PS2 = 0xF8,
    KBD_ENC_CMD_ALL_SEND_MAKE = 0xF9,
    KBD_ENC_CMD_ALL_ALL_MAKE_BREAK = 0xFA,
    KBD_ENC_CMD_SET_KEY_AUTOREPEAT = 0xFB,
    KBD_ENC_CMD_SET_BREAK = 0xFD,
    KBD_ENC_CMD_RESEND = 0xFE,
    KBD_ENC_CMD_RESET_POST = 0xFF
};

enum KBD_MODIFIER_KEYS
{
    KBD_MOD_CAPS_LOCK = 1,
    KBD_MOD_SCROLL_LOCK = 2,
    KBD_MOD_NUM_LOCK = 4,
    KBD_MOD_SHIFT = 8,
    KBD_MOD_CTRL = 16,
    KBD_MOD_ALT = 32,

};

static volatile char _KBD_READY_FLAG = 0;
unsigned static volatile char ModifierKeyStatus = 0;

// Efficiency of this can be improved through bitmasks 1k = 1b

unsigned char KBD_ReadCTRLStatus();
unsigned char KBD_ReadEncoderBuffer();
void KBD_SendCTRLCommand(unsigned char Command);
void KBD_SendEncoderCommand(unsigned char Command);
void KBD_WaitACK();

void KBD_Init(void)
{
    ModifierKeyStatus = 0;
    KBD_Enable();
}

inline unsigned char KBD_ReadCTRLStatus()
{
    return inb(KBD_CTRL_STATS_REGISTER);
}

void KBD_Enable(void)
{
    outb(0x21, 0xFD);
}

void KBD_SendCTRLCommand(unsigned char Command)
{
    while (1)
    {
        if ((KBD_ReadCTRLStatus() & KYBRD_CTRL_STATS_IN_BUF) == 0)
        {
            break;
        }
        outb(KBD_CTRLE_CMD_REGISTER, Command);
    }
}

inline unsigned char KBD_ReadEncoderBuffer()
{
    return inb(KBD_ENC_INPUT_BUFFER);
}

void KBD_SendEncoderCommand(unsigned char Command)
{
    while (1)
    {
        if ((KBD_ReadCTRLStatus() & KYBRD_CTRL_STATS_IN_BUF) == 0)
        {
            break;
        }
        outb(KBD_ENC_CMD_REGISTER, Command);
    }
}

void KBD_SetLEDS(unsigned char NumLock, unsigned char CapsLock, unsigned char ScrollLock)
{
    char Data = 0;
    Data = ModifierKeyStatus | 1;
    Data = ModifierKeyStatus | 2;
    Data = ModifierKeyStatus | 4;
    // KBD_SendEncoderCommand(KBD_ENC_CMD_SET_LEDS);
    outb(0x60, 0xED);
    KBD_WaitACK();
    outb(0x60, Data);
}

/* Example of how keyboard handling can be done */
// void KBD_ReadInput(unsigned char *Scancode, unsigned char *Key, unsigned char *State)
// {
//     _KBD_READY_FLAG = 0;
//     *Scancode = inb(KBD_ENC_INPUT_BUFFER);
//     unsigned char Extended = (*Scancode == 0xE0) ? 1 : 0;
//     *Scancode = Extended ? inb(KBD_ENC_INPUT_BUFFER) : *Scancode;
//     if ((*Scancode) & 0x80)
//     {
//         *State = KBD_UP;
//     }
//     else
//     {
//         *State = KBD_DOWN;
//     }
//     *Key = ((ModifierKeyStatus & KBD_MOD_SHIFT) || (ModifierKeyStatus & KBD_MOD_CAPS_LOCK)) ? kbduk_shift[*Scancode] : kbduk[*Scancode];

//     if (Extended)
//     {
//         // Right control
//         if (*Scancode == 0x1D)
//         {
//             ModifierKeyStatus |= KBD_MOD_CTRL;
//         }
//         else if (*Scancode == 0x9D)
//         {
//             ModifierKeyStatus &= !KBD_MOD_CTRL;
//         }

//         // Right Alt
//         else if (*Scancode == 0x38)
//         {
//             ModifierKeyStatus |= KBD_MOD_ALT;
//         }
//         else if (*Scancode == 0xB8)
//         {
//             ModifierKeyStatus &= !KBD_MOD_ALT;
//         }

//         // Other extended keys
//     }
//     else
//     {

//         // Left Shift
//         if (*Scancode == 0x2A)
//         {
//             ModifierKeyStatus |= KBD_MOD_SHIFT;
//         }
//         else if (*Scancode == 0xAA)
//         {
//             ModifierKeyStatus &= !KBD_MOD_SHIFT;
//         }
//         // Right shift
//         else if (*Scancode == 0x36)
//         {
//             ModifierKeyStatus |= KBD_MOD_SHIFT;
//         }
//         else if (*Scancode == 0xB6)
//         {
//             ModifierKeyStatus &= !KBD_MOD_SHIFT;
//         }

//         // Left control
//         else if (*Scancode == 0x1D)
//         {
//             ModifierKeyStatus |= KBD_MOD_CTRL;
//         }
//         else if (*Scancode == 0x9D)
//         {
//             ModifierKeyStatus &= !KBD_MOD_CTRL;
//         }

//         // Left Alt
//         else if (*Scancode == 0x38)
//         {
//             ModifierKeyStatus |= KBD_MOD_ALT;
//         }
//         else if (*Scancode == 0xB8)
//         {
//             ModifierKeyStatus &= !KBD_MOD_ALT;
//         }

//         // Caps lock -- A toggle key
//         else if (*Scancode == 0x3A)
//         {
//             ModifierKeyStatus ^= KBD_MOD_CAPS_LOCK;
//         }

//         // There might be missing keys... Enjoy finding them :)
//     }
// }

// void KeyboardHandler(void)
// {
//     _KBD_READY_FLAG = 1;
//     outb(0x20, 0x20); //EOI
// }

void KBD_WaitACK()
{
    while (!(inb(0x60) == 0xFA))
        ;
}