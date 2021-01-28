// test for Trident 8900 cards
#include	<conio.h>
#include	<dos.h>

#define LOWORD(l)           ((int)(l))
#define HIWORD(l)           ((int)((l) >> 16))

static	int	curBank = 0;

void	setTridentMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h

		mov	dx, 3CEh	// set pagesize to 64k
		mov	al, 6
		out	dx, al
		inc	dx
		in	al, dx
		dec	dx
		or	al, 4
		mov	ah, al
		mov	al, 6
		out	dx, ax
		mov	dx, 3C4h	// set to BPS mode
		mov	al, 0Bh
		out	dx, al
		inc	dx
		in	al, dx
	}
}

void	setTridentBank ( int start )
{
	if ( start == curBank )
		return;

	curBank = start;
	asm {
		mov	dx, 3C4h
		mov	al, 0Bh
		out	dx, al
		inc	dx
		mov	al, 0
		out	dx, al
		in	al,dx
		dec	dx
		mov	al, 0Eh
		mov	ah, byte ptr start
		xor	ah, 2
		out	dx, ax
	}
}

void	writePixel ( int x, int y, int color )
{
	long	addr = 640l * (long)y + (long)x;

	setTridentBank ( HIWORD ( addr ) );
	pokeb ( 0xA000, LOWORD ( addr ), color );
}

main ()
{
	setTridentMode ( 0x5D );	// 640x480x256

	for ( int i = 0; i < 640; i++ )
		for ( int j = 0; j < 480; j++ )
			writePixel ( i, j, ((i/20)+1)*(j/20+1) );

	getch ();
}
