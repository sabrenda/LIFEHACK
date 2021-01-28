//
// Author: Alex V. Boreskoff
//
#include	<dos.h>
#include	<mem.h>
#include	"keyboard.h"

void interrupt (*Keyboard::oldKbdHandler)(...);

int	Keyboard :: keys [128];

int	Keyboard :: scanCode [256] =
{
	0,  0,  0,  0,  0,  0,  0, 15,
	14, 0,  0,  0,  0, 28,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  1,  0,  0,  0,  0,
	57, 0,  0,  0,  0,  0,  0, 40,
	0,  0, 55, 78, 51, 12, 52, 53,
	11, 2,  3,  4,  5,  6,  7,  8,
	9,  10, 0,  39, 0, 13,  0,  0,
	0,  30, 48, 46, 32, 18, 33, 34,
	35, 23, 36, 37, 38, 50, 49, 24,
	25, 16, 19, 31, 20, 22, 47, 17,
	45, 21, 44, 26, 43, 27, 0,  0,
	41, 30, 48, 46, 32, 18, 33, 34,
	35, 23, 36, 37, 38, 50, 49, 24,
	25, 16, 19, 31, 20, 22, 47, 17,
	45, 21, 44,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	0,   0,  0,  0,  0,  0,  0,  0,
	56,  0,  0, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 87, 88, 71,
	72, 73, 74, 75,  0, 77, 78, 79,
	80, 81, 82, 83
};

int	Keyboard :: charCode [128] =
{
	  0,  27,  49,  50,  51,  52, 53,  54,
	 55,  56,  57,  48,  45,  61,  8,   9,
	113, 119, 101, 114, 116, 121,117, 105,
	111, 112,  91,  93,  13,   0, 97, 115,
	100, 102, 103, 104, 106, 107,108,  59,
	 39,  96,   0,  92, 122, 120, 99, 118,
	 98, 110, 109,  44,  46,   47, 0,  42,
	  0,  32,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,  45,   0,   0,   0, 43,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0,
	  0,   0,   0,   0,   0,   0,  0,   0
};

Keyboard :: Keyboard ()
{
	oldKbdHandler = getvect ( 9 );	// save old keyboard ISR

	setvect ( 9, newKbdHandler );	// install new handler
	memset  ( keys, '\0', sizeof ( keys ) );
}

Keyboard :: ~Keyboard ()
{
	setvect ( 9, oldKbdHandler );
}

void	interrupt Keyboard :: newKbdHandler (...)
{
	int	scanCode  = inportb ( 0x60 );
	int	otherCode = inportb ( 0x61 );
				// acknowledge keyboard and
				// interrupt controllers
	outportb ( 0x61, otherCode | 0x80 );
	outportb ( 0x61, otherCode );
	outportb ( 0x20, 0x20 );

				// set key state
	keys [scanCode & 0x7F] = ( ( scanCode & 0x80 ) == 0 );
}

int	Keyboard :: getScanCode ( int key ) const
{
	if ( key < 0x0100 )
		return scanCode [key];

	return key >> 8;
}
