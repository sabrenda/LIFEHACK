// test for Oak cards
#include	<conio.h>
#include	<dos.h>
#include	<stdio.h>

#define	OAK_037C	1
#define	OAK_067		2
#define	OAK_077		3

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

	return TestPort ( port + 1, mask );
}

int	findOak ()
{
	if ( !testReg ( 0x3DE, 0x0D, 0x38 ) )
		return 0;

	if ( !testReg ( 0x3DE, 0x11, 0xFF ) )	// OTI-037C bank switching registers are read only We have an Oak Technologies SuperVGA, so determine the type. The OTI-037C
		return OAK_037C;

	if ( ( readReg ( 0x3DE, 0x0B ) >> 5 ) == 5 )
		return OAK_077;
	else
		return OAK_067;
}

void	setOakMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

void	setOakBank ( int start )
{
	if ( start == curBank )
		return;

	curBank = start;
	asm {
		mov	dx, 3CEh
		mov	ax, start
		and	al, 0Fh
		mov	ah, al
		mov	cl, 4
		shl	al, cl
		or	ah, al
		mov	al, 11h
		out	dx, ax
	}
}

void	writePixel ( int x, int y, int color )
{
	long	addr = 640l * (long)y + (long)x;

	setOakBank ( HIWORD ( addr ) );
	pokeb ( 0xA000, LOWORD ( addr ), color );
}

main ()
{
	if ( !findOak () )
	{
		printf ( "\nOak card not found" );

		return 1;
	}

	setOakMode ( 0x53 );	// 640x480x256

	for ( int i = 0; i < 640; i++ )
		for ( int j = 0; j < 480; j++ )
			writePixel ( i, j, ((i/20)+1)*(j/20+1) );

	getch ();
}
