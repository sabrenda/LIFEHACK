#include	<bios.h>
#include	<conio.h>
#include	"mouse.h"

unsigned short andMask [] =
{
	0x0FFF, 0x07FF, 0x01FF, 0x007F,
	0x801F, 0xC007, 0xC001, 0xE000,
	0xE0FF, 0xF0FF, 0xF0FF, 0xF8FF,
	0xF8FF, 0xFCFF, 0xFCFF, 0xFEFF
};

unsigned short xorMask [] =
{
	0x0000, 0x6000, 0x7800, 0x3E00,
	0x3F80, 0x1FE0, 0x1FF8, 0x0FFE,
	0x0F00, 0x0700, 0x0700, 0x0300,
	0x0300, 0x0100, 0x0100, 0x0000,
};

CursorShape	cursor ( andMask, xorMask, Point ( 1, 1 ) );

int	doneFlag = 0;

void	setVideoMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

#pragma argsused
void	cdecl far waitPress ( int mask, int button, int x, int y )
{
	if ( mask & MOUSE_RBUTTON_PRESS )
		doneFlag = 1;
}

main ()
{
	Point	p ( 0, 0 );

	setVideoMode ( 0x12 );
	resetMouse ();
	showMouseCursor ();
	setMouseShape ( cursor );
	setMouseHandler ( waitPress );
	moveMouseCursor ( p );

	while ( !doneFlag )
		;

	hideMouseCursor ();
	setVideoMode ( 3 );

	return 0;
}
