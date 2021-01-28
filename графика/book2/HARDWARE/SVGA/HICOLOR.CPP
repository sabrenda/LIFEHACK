// test for VESA
#include	<conio.h>
#include	<dos.h>
#include	<stdio.h>
#include	<string.h>
#include	"Vesa.h"

#define LOWORD(l)           ((int)(l))
#define HIWORD(l)           ((int)((l) >> 16))

inline	int	RGBColor ( int red, int green, int blue )
{
	return ( ( red >> 3 ) << 10 ) | ( ( green >> 3 ) << 5 ) | ( blue >> 3 );
}

static	int		curBank     = 0;
static	int		granularity = 1;
static	VESAModeInfo	curMode;

int	findVESA ( VESAInfo& vi )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr vi
		mov	ax, 4F00h
		int	10h
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr vi
		mov	ax, 4F00h
		int	10h
		pop	di
	}
#endif
	if ( _AX != 0x004F )
		return 0;

	return !strncmp ( vi.sign, "VESA", 4 );
}

int	findVESAMode ( int mode, VESAModeInfo& mi )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr mi
		mov	ax, 4F01h
		mov	cx, mode
		int	10h
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr mi
		mov	ax, 4F01h
		mov	cx, mode
		int	10h
		pop	di
	}
#endif
	return _AX == 0x004F;

}

int	setVESAMode ( int mode )
{
	if ( !findVESAMode ( mode, curMode ) )
		return 0;

	granularity = 64 / curMode.winGranularity;
	asm {
		mov	ax, 4F02h
		mov	bx, mode
		int	10h
	}

	return _AX == 0x004F;
}

int	getVESAMode ()
{
	asm {
		mov	ax, 4F03h
		int	10h
	}

	if ( _AX != 0x004F )
		return 0;
	else
		return _BX;
}

void	setVESABank ( int start )
{
	if ( start == curBank )
		return;

	curBank = start;
	start  *= granularity;
	asm {
		mov	ax, 4F05h
		mov	bx, 0
		mov	dx, start
		push	dx
		int	10h
		mov	bx, 1
		pop	dx
		int	10h
	}
}

void	writePixel ( int x, int y, int color )
{
	long	addr = (long)curMode.bytesPerScanLine * (long)y + (long)(x<<1);

	setVESABank ( HIWORD ( addr ) );
	poke ( 0xA000, LOWORD ( addr ), color );
}

main ()
{
	VESAInfo info;

	if ( !findVESA ( info ) )
	{
		printf ( "VESA VBE not found" );

		return 1;
	}

	if ( !setVESAMode ( VESA_640x480x32K ) )
	{
		printf ( "Mode not supported" );

		return 1;
	}

	for ( int i = 0; i < 256; i++ )
		for ( int j = 0; j < 256; j++ )
		{
			writePixel ( 320-i, 240-j, RGBColor ( 0,j,i) );
			writePixel ( 320+i, 240-j, RGBColor ( i,j,i) );
			writePixel ( 320+i, 240+j, RGBColor ( j,i,i) );
			writePixel ( 320-i, 240+j, RGBColor ( j,0,i) );
		}

	getch ();
}
