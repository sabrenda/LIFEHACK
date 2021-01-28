#include	<dos.h>
#include	"Ega.h"

int	findEGA ()
{
	asm {
		mov	ax, 1200h
		mov	bx, 10h
		int	10h
	}

	return _BL != 0x10;
}

int	findVGA ()
{
	asm {
		mov	ax, 1A00h
		int	10h
	}

	return _AL == 0x1A;
}

void	setVideoMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

void	setVisiblePage ( int page )
{
	asm {
		mov	ah, 5
		mov	al, byte ptr page
		int	10h
	}
}

char far * findROMFont ( int size )
{
	int	b = ( size == 16 ? 6 : 2 );

	asm {
		push	es
		push	bp
		mov	ax, 1130h
		mov	bh, byte ptr b
		mov	bl, 0
		int	10h
		mov	ax, es
		mov	bx, bp
		pop	bp
		pop	es
	}

	return (char far *) MK_FP ( _AX, _BX );
}

void	setPalette ( RGB far * palette, int size )
{
	asm {
		push	es
		mov	ax, 1012h
		mov	bx, 0			// first color to set
		mov	cx, size		// # of colors
		les	dx, palette		// ES:DX == table of color values
		int	10h
		pop	es
	}
}

void	getPalette ( RGB far * palette )
{
	asm {
		push	es
		mov	ax, 1017h
		mov	bx, 0			// first color to set
		mov	cx, 256			// # of colors
		les	dx, palette		// ES:DX == table of color values
		int	10h
		pop	es
	}
}
