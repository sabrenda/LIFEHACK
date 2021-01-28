//
// Author: Alex V. Boreskoff
//

#ifndef	__KEYBOARD__
#define	__KEYBOARD__
				// key (scan) codes for various keys
#define	KC_SYSREQ	0x54
#define	KC_CAPSLOCK	0x3A
#define	KC_NUMLOCK	0x45
#define	KC_SCROLLLOCK	0x46
#define	KC_LEFTCTRL	0x1D
#define	KC_LEFTALT	0x38
#define	KC_LEFTSHIFT	0x2A
#define	KC_RIGHTSHIFT	0x36
#define	KC_ESC		0x01
#define	KC_BACKSPACE	0x0E
#define	KC_ENTER	0x1C
#define	KC_SPACE	0x39
#define	KC_TAB		0x0F
#define	KC_F1		0x3B
#define	KC_F2		0x3C
#define	KC_F3		0x3D
#define	KC_F4		0x3E
#define	KC_F5		0x3F
#define	KC_F6		0x40
#define	KC_F7		0x41
#define	KC_F8		0x42
#define	KC_F9		0x43
#define	KC_F10		0x44
#define	KC_F11		0x57
#define	KC_F12		0x58
#define	KC_A		0x1E
#define	KC_B		0x30
#define	KC_C		0x2E
#define	KC_D		0x20
#define	KC_E		0x12
#define	KC_F		0x21
#define	KC_G		0x22
#define	KC_H		0x23
#define	KC_I		0x17
#define	KC_J		0x24
#define	KC_K		0x25
#define	KC_L		0x26
#define	KC_M		0x32
#define	KC_N		0x31
#define	KC_O		0x18
#define	KC_P		0x19
#define	KC_Q		0x10
#define	KC_R		0x13
#define	KC_S		0x1F
#define	KC_T		0x14
#define	KC_U		0x16
#define	KC_V		0x2F
#define	KC_W		0x11
#define	KC_X		0x2D
#define	KC_Y		0x15
#define	KC_Z		0x2C
#define	KC_1		0x02
#define	KC_2		0x03
#define	KC_3		0x04
#define	KC_4		0x05
#define	KC_5		0x06
#define	KC_6		0x07
#define	KC_7		0x08
#define	KC_8		0x09
#define	KC_9		0x0A
#define	KC_0		0x0B
#define	KC_MINUS	0x0C
#define	KC_EQUAL	0x0D
#define	KC_LBRACKET	0x1A
#define	KC_RBRACKET	0x1B
#define	KC_SEMICOLON	0x27
#define	KC_TICK		0x28
#define	KC_APOSTROPHE	0x29
#define	KC_BACKSLASH	0x2B
#define	KC_COMMA	0x33
#define	KC_PERIOD	0x34
#define	KC_SLASH	0x35
#define	KC_KEYPAD0	0x52
#define	KC_KEYPAD1	0x4F
#define	KC_KEYPAD2	0x50
#define	KC_KEYPAD3	0x51
#define	KC_KEYPAD4	0x4B
#define	KC_KEYPAD5	0x4C
#define	KC_KEYPAD6	0x4D
#define	KC_KEYPAD7	0x47
#define	KC_KEYPAD8	0x48
#define	KC_KEYPAD9	0x49
#define	KC_KEYPADDEL	0x53
#define	KC_KEYPADSTAR	0x37
#define	KC_KEYPADMINUS	0x4A
#define	KC_KEYPADPLUS	0x4E
#define	KC_KEYPADENTER	0x9C
#define	KC_KEYPADSLASH	0xB5

class	Keyboard
{
	static	void interrupt (*oldKbdHandler)(...);
	static	int              keys     [128];
	static	int	  	 scanCode [256];
	static	int		 charCode [128];
	static void interrupt    newKbdHandler (...);

public:
	Keyboard ();
	~Keyboard ();

	int	isPressed ( int key ) const
	{
		return keys [key];
	}

	int	getChar ( int scanCode ) const
	{
		return charCode [scanCode & 0x7F];
	}

	int	getScanCode ( int key ) const;
};

#endif
