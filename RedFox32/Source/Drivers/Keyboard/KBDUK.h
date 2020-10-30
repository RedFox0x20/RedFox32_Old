#ifndef KBD_UK
#define KBD_UK

/* ----------------- Minor innacuracies due to US scan codes ---------------- */

enum KBD_UK_SCANCODES
{
    KBD_UK_ESCAPE = 0x01,
    KBD_UK_1,
    KBD_UK_2,
    KBD_UK_3,
    KBD_UK_4,
    KBD_UK_5,
    KBD_UK_6,
    KBD_UK_7,
    KBD_UK_8,
    KBD_UK_9,
    KBD_UK_0,
    KBD_UK_DASH,
    KBD_UK_EQUALS,
    KBD_UK_BACKSPACE,
    KBD_UK_TAB,
    KBD_UK_Q,
    KBD_UK_W,
    KBD_UK_E,
    KBD_UK_R,
    KBD_UK_T,
    KBD_UK_Y,
    KBD_UK_U,
    KBD_UK_I,
    KBD_UK_O,
    KBD_UK_P,
    KBD_UK_LEFT_SQUARE_BRACKET,
    KBD_UK_RIGHT_SQUARE_BRACKET,
    KBD_UK_ENTER,
    KBD_UK_CONTROL, // Which?
    KBD_UK_A,
    KBD_UK_S,
    KBD_UK_D,
    KBD_UK_F,
    KBD_UK_G,
    KBD_UK_H,
    KBD_UK_J,
    KBD_UK_K,
    KBD_UK_L,
    KBD_UK_SEMI_COLON,
    KBD_UK_BACKSLASH,
    KBD_UK_GRAVE,
    KBD_UK_LEFT_SHIFT,
    KBD_UK_HASH,
    KBD_UK_Z,
    KBD_UK_X,
    KBD_UK_C,
    KBD_UK_V,
    KBD_UK_B,
    KBD_UK_N,
    KBD_UK_M,
    KBD_UK_COMMA,
    KBD_UK_PERIOD,
    KBD_UK_FORWARDSLASH,
    KBD_UK_RIGHT_SHIFT,
    KBD_UK_ASTERISK,
    KBD_UK_LEFT_ALT,
    KBD_UK_SPACE,
    KBD_UK_CAPS_LOCK,
    KBD_UK_F1,
    KBD_UK_F2,
    KBD_UK_F3,
    KBD_UK_F4,
    KBD_UK_F5,
    KBD_UK_F6,
    KBD_UK_F7,
    KBD_UK_F8,
    KBD_UK_F9,
    KBD_UK_F10,
    KBD_UK_NUM_LOCK,
    KBD_UK_SCROLL_LOCK,
    KBD_UK_HOME,
    KBD_UK_UP_ARROW,
    KBD_UK_PAGE_UP,
    KBD_UK_NUMPAD_MINUS,
    KBD_UK_LEFT_ARROW,
    KBD_UK_UNKNOWNKEY,
    KBD_UK_RIGHT_ARROW,
    KBD_UK_NUMPAD_PLUS,
    KBD_UK_END,
    KBD_UK_DOWN_ARROW,
    KBD_UK_PAGE_DOWN,
    KBD_UK_INSERT,
    KBD_UK_DELETE,
    KBD_UK_UNKNOWNKEY2,
    KBD_UK_UNKNOWNKEY3,
    KBD_UK_UNKNOWNKEY4,
    KBD_UK_F11,
    KBD_UK_F12,
    // Aything else is invalid
};

static const unsigned char kbduk[128] =
    {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
        '9', '0', '-', '=', '\b',                         /* Backspace */
        '\t',                                             /* Tab */
        'q', 'w', 'e', 'r',                               /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
        0,                                                /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
        '\'', '`', 0,                                     /* Left shift */
        '#', 'z', 'x', 'c', 'v', 'b', 'n',                /* 49 */
        'm', ',', '.', '/', 0,                            /* Right shift */
        '*',
        0,   /* Left Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

static const unsigned char kbduk_shift[128] =
    {
        0, 27, '!', '"', 1, '$', '%', '^', '&', '*',      /* 9 */
        '(', ')', '_', '+', '\b',                         /* Backspace */
        '\t',                                             /* Tab */
        'Q', 'W', 'E', 'R',                               /* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',     /* Enter key */
        0,                                                /* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
        '@', '\\', 0,                                     /* Left shift */
        '~', 'Z', 'X', 'C', 'V', 'B', 'N',                /* 49 */
        'M', '<', '>', '/', 0,                            /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
};

#endif