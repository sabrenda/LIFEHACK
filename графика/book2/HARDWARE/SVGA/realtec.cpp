// test for Realtec cards
#include	<conio.h>
#include	<dos.h>
#include	<process.h>
#include	<stdio.h>

#define LOWORD(l)           ((int)(l))
#define HIWORD(l)           ((int)((l) >> 16))

static	int	curBank = 0;

void	setRealtecMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

void	setRealtecBank ( int start )
{
	if ( start == curBank )
		return;

	curBank = start;
	asm {
		mov	ax, start
		and	al, 0Fh
		mov	dx, 3D7h
		out	dx, al
	}
}

void	writePixel ( int x, int y, int color )
{
	long	addr = 640l * (long)y + (long)x;

	setRealtecBank ( HIWORD ( addr ) );
	pokeb ( 0xA000, LOWORD ( addr ), color );
}

main ()
{
	setRealtecMode ( 0x26 );	// 640x480x256

	for ( int i = 0; i < 640; i++ )
		for ( int j = 0; j < 480; j++ )
			writePixel ( i, j, ((i/20)+1)*(j/20+1) );

	getch ();
}
