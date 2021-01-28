#include	<alloc.h>
#include	<conio.h>
#include	<stdio.h>
#include	"Ega.h"

int	readPixel ( int x, int y )
{
	int 	   color = 0;
	char far * vptr = (char far *) MK_FP ( 0xA000, y*80 + (x >> 3) );
	char	   mask = pixelMask ( x );

	for ( int plane = 3; plane >= 0; plane-- )
	{
		writeReg ( EGA_GRAPHICS, EGA_READ_MAP_SELECT, plane );
		color <<= 1;
		if ( *vptr & mask )
			color |= 1;
	}

	return color;
}

void	writePixel ( int x, int y, int color )
{
	char far * vptr = (char far *) MK_FP ( 0xA000, y*80 + (x >> 3) );

	writeReg ( EGA_GRAPHICS, EGA_ENABLE_SET_RESET, 0x0F );	// enable all planes
	writeReg ( EGA_GRAPHICS, EGA_SET_RESET, color );
	writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, pixelMask ( x ) );

	*vptr += 1;

	writeReg ( EGA_GRAPHICS, EGA_ENABLE_SET_RESET, 0 );	// disable all planes
	writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, 0xFF );		// restore reg
}

int	findPixel ( int x1, int x2, int y, int color )
{
	char far * vptr  = (char far *) MK_FP ( 0xA000, y*80 + (x1 >> 3) );
	int	   cols  = ( x2 >> 3 ) - ( x1 >> 3 ) - 1;
	char	   lmask = leftMask ( x1 );
	char	   rmask = rightMask ( x2 );
	char	   mask;

	setRWMode ( 1, 0 );
	writeReg ( EGA_GRAPHICS, EGA_COLOR_COMPARE, color );

	if ( cols < 0 )
		return *vptr & lmask & rmask;

	if ( mask = *vptr++ & lmask )
		return mask;

	while ( cols-- > 0 )
		if ( mask = *vptr++ )
			return mask;

	return *vptr & rmask;
}

void	copyRect ( int x1, int y1, int x2, int y2, int x, int y )
{
	char far * src  = (char far *) MK_FP ( 0xA000, y1*80 + (x1 >> 3) );
	char far * dst  = (char far *) MK_FP ( 0xA000, y*80 + (x >> 3) );
	int	   cols  = ( x2 >> 3 ) - ( x1 >> 3 );

	setRWMode ( 0, 1 );

	for ( int i = y1; i <= y2; i++ )
	{
		for ( int j = 0; j < cols; j++ )
			*dst++ = *src++;

		src += 80 - cols;
		dst += 80 - cols;
	}

	setRWMode ( 0, 0 );
}

void	bar ( int x1, int y1, int x2, int y2, int color )
{
	char far * vptr  = (char far *) MK_FP ( 0xA000, y1*80 + (x1 >> 3) );
	int	   cols  = ( x2 >> 3 ) - ( x1 >> 3 ) - 1;
	char	   lmask = leftMask ( x1 );
	char	   rmask = rightMask ( x2 );
	char	   latch;

	setRWMode ( 0, 2 );

	if ( cols < 0 )		// both x1 & x2 are located in the same byte
	{
		writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, lmask & rmask );

		for ( int y = y1; y <= y2; y++, vptr += 80 )
		{
			latch = *vptr;
			*vptr = color;
		}

		writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, 0xFF );
	}
	else
	{
		for ( int y = y1; y <= y2; y++ )
		{
			writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, lmask );
			latch = *vptr;
			*vptr++ = color;

			writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, 0xFF );
			for ( int x = 0; x < cols; x++ )
			{
				latch = *vptr;
				*vptr++ = color;
			}

			writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, rmask );
			latch = *vptr;
			*vptr++ = color;

			vptr += 78 - cols;
		}
	}

	setRWMode ( 0, 0 );
	writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, 0xFF );
}

void	storeRect ( int x1, int y1, int x2, int y2, char huge * buf )
{
	char far * vptr  = (char far *) MK_FP ( 0xA000, y1*80 + (x1 >> 3) );
	int	   cols  = ( x2 >> 3 ) - ( x1 >> 3 ) - 1;

	if ( cols < 0 )
		cols = 0;

	for ( int y = y1; y <= y2; y++, vptr += 80 )
		for ( int plane = 0; plane < 4; plane++ )
		{
			writeReg ( EGA_GRAPHICS, EGA_READ_MAP_SELECT, plane );

			for ( int x = 0; x < cols + 2; x++ )
				*buf++ = *vptr++;

			vptr -= cols + 2;
		}
}

void	restoreRect ( int x1, int y1, int x2, int y2, char huge * buf )
{
	char far * vptr  = (char far *) MK_FP ( 0xA000, y1*80 + (x1 >> 3) );
	int	   cols  = ( x2 >> 3 ) - ( x1 >> 3 ) - 1;
	char	   lmask = leftMask ( x1 );
	char	   rmask = rightMask ( x2 );
	char	   latch;

	if ( cols < 0 )
	{
		lmask &= rmask;
		rmask  = 0;
		cols   = 0;
	}

	for ( int y = y1; y <= y2; y++, vptr += 80 )
		for ( int plane = 0; plane < 4; plane++ )
		{
			writeReg ( EGA_GRAPHICS,  EGA_BIT_MASK, lmask );
			writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 1 << plane );

			latch = *vptr;
			*vptr++ = *buf++;

			writeReg ( EGA_GRAPHICS, EGA_BIT_MASK, 0xFF );

			for ( int x = 0; x < cols; x++ )
				*vptr++ = *buf++;

			writeReg ( EGA_GRAPHICS,  EGA_BIT_MASK, rmask );

			latch = *vptr;
			*vptr++ = *buf++;

			vptr -= cols + 2;
		}

		writeReg ( EGA_GRAPHICS,  EGA_BIT_MASK, 0xFF );
		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
}

main ()
{
	if ( !findEGA () )
	{
		printf ( "\nEGA compatible card not found." );
		return -1;
	}

	setVideoMode ( 0x10 );		// set 640x350 16 colors mode

	for ( int i = 0; i < 128; i++ )
		writePixel ( i, 0, ( i >> 2 ) & 0x0F );

	setWriteMode ( 3 );			// set XOR write mode

	for ( i = 1; i < 200; i += 10 )
		bar ( i, i, i + 45, i + 55, i & 0x0F );

	setWriteMode ( 0 );

	char huge * buf = (char huge *)farmalloc ( 60000U );

	storeRect ( 0, 0, 100, 100, buf );
	getch ();
	restoreRect ( 240, 20, 340, 120, buf );
	getch ();
	farfree ( buf );

	bar ( 500, 200, 600, 300, MAGENTA );
	bar ( 400, 100, 500, 150, YELLOW );

	getch ();
	setVideoMode ( 3 );
}
