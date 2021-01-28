// test for Cirrus Logic 52xx cards
#include	<conio.h>
#include	<dos.h>
#include	<process.h>
#include	<stdio.h>

#define LOWORD(l)           ((int)(l))
#define HIWORD(l)           ((int)((l) >> 16))

inline	void	writeReg ( int base, int reg, int value )
{
	outportb ( base,     reg );
	outportb ( base + 1, value );
}

inline	char	readReg ( int base, int reg )
{
	outportb ( base, reg );

	return inportb ( base + 1 );
}

static	int	curBank = 0;

// check bits specified by mask in port for being readable/writable
int	testPort ( int port, char mask )
{
	char	save = inportb ( port );

	outportb ( port, save & ~mask );

	char	v1 = inportb ( port ) & mask;

	outportb ( port, save | mask );

	char	v2 = inportb ( port ) & mask;

	outportb ( port, save );

	return v1 == 0 && v2 == mask;
}

int	testReg ( int port, int reg, char mask )
{
	outportb ( port, reg );

	return testPort ( port + 1, mask );
}

int	findCirrus ()
{
	char	save = readReg ( 0x3C4, 6 );
	int	res  = 0;

	writeReg ( 0x3C4, 6, 0x12 );		// enable extended registers

	if ( readReg ( 0x3C4, 6 ) == 0x12 )
		if ( testReg ( 0x3C4, 0x1E, 0x3F ) && testReg ( 0x3D4, 0x1B, 0xFF ) )
			res = 1;

	writeReg ( 0x3C4, 6, save );

	return res;
}

void	setCirrusMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h

		mov	dx, 3C4h	// enable extended registers
		mov	al, 6
		out	dx, al
		inc	dx
		mov	al, 12h
		out	dx, al
	}
}

void	setCirrusBank ( int start )
{
	if ( start == curBank )
		return;

	curBank = start;
	asm {
		mov	dx, 3CEh
		mov	al, 9
		mov	ah, byte ptr start
		mov	cl, 4
		shl	ah, cl
		out	dx, ax
	}
}

void	writePixel ( int x, int y, int color )
{
	long	addr = 640l * (long)y + (long)x;

	setCirrusBank ( HIWORD ( addr ) );
	pokeb ( 0xA000, LOWORD ( addr ), color );
}

main ()
{
	if ( !findCirrus () )
	{
		printf ( "\nCirrus card not found" );
		exit ( 1 );
	}

	setCirrusMode ( 0x5F );	// 640x480x256

	for ( int i = 0; i < 640; i++ )
		for ( int j = 0; j < 480; j++ )
			writePixel ( i, j, ((i/20)+1)*(j/20+1) );

	getch ();
}
