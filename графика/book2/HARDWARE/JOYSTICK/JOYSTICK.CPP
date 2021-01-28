#include	<dos.h>
#include	"joystick.h"

unsigned joy1MinX, joy1MaxX, joy1MinY, joy1MaxY;
unsigned joy2MinX, joy2MaxX, joy2MinY, joy2MaxY;

unsigned joystickButtons ( char button )
{
	outportb ( JOYPORT, 0 );

	return ~inportb ( JOYPORT ) & button;
}

unsigned joystickValue   ( char stick  )
{
	asm {
		cli
		mov	ah, byte ptr stick
		xor	al, al
		xor	cx, cx
		mov	dx, JOYPORT
		out	dx, al
	}
discharge:
	asm {
		in	al, dx
		test	al, ah
		loopne	discharge

		sti
		xor	ax, ax
		sub	ax, cx
	}
}

unsigned joystickValueBIOS   ( char stick  )
{
	REGS	inregs, outregs;

	inregs.h.ah = 0x84;
	inregs.x.dx = 0x01;

	int86 ( 0x15, &inregs, &outregs );

	switch ( stick )
	{
		case JOYSTICK_1_X:
			return outregs.x.ax;

		case JOYSTICK_1_Y:
			return outregs.x.bx;

		case JOYSTICK_2_X:
			return outregs.x.cx;

		case JOYSTICK_2_Y:
			return outregs.x.dx;
	}

	return 0;
}

int	joystickPresent ( char stick )
{
	asm {
		mov	ah, 84h
		mov	dx, 1
		int	15h		// call BIOS to read joystick values
	}

	if ( _AX == 0 && _BX == 0 && stick == 1 )
		return 0;

	if ( _CX == 0 && _DX == 0 && stick == 2 )
		return 0;

	return 1;
}
