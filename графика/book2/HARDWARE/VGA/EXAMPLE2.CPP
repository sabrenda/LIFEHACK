#include	<alloc.h>
#include	<conio.h>
#include	<mem.h>
#include	<stdio.h>
#include	"ega.h"

unsigned   pageBase = 0;
char       leftPlaneMask  [] = { 0x0F, 0x0E, 0x0C, 0x08 };
char	   rightPlaneMask [] = { 0x01, 0x03, 0x07, 0x0F };
char far * font;

void	setX ()
{
	setVideoMode ( 0x13 );

	pageBase = 0xA000;

	writeReg ( EGA_SEQUENCER, 4, 6 );
	writeReg ( EGA_CRTC, 0x17, 0xE3 );
	writeReg ( EGA_CRTC, 0x14, 0 );

				// clear screen
	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
	_fmemset ( MK_FP ( pageBase, 0 ), '\0', 0xFFFF );
}

void	setVisualPage ( int page )
{
	unsigned addr = page * 0x4000;

				// wait for vertical retrace
	while ( ( inportb ( 0x3DA ) & 0x08 ) == 0 )
		;

	writeReg ( EGA_CRTC, 0x0C, addr >> 8 );
	writeReg ( EGA_CRTC, 0xDC, addr & 0x0F );
}

void	setActivePage ( int page )
{
	pageBase = 0xA000 + page * 0x400;
}

void	writePixel ( int x, int y, int color )
{
	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 1 << ( x & 3 ) );

	pokeb ( pageBase, y*80 + ( x >> 2 ), color );

	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
}

int	readPixel ( int x, int y )
{
	writeReg ( EGA_GRAPHICS, EGA_READ_MAP_SELECT, x & 3 );

	return peekb ( pageBase, y*80 + ( x >> 2 ) );
}

void	bar ( int x1, int y1, int x2, int y2, int color )
{
	char far * videoPtr = (char far *) MK_FP ( pageBase, y1*80 + (x1 >> 2) );
	char far * ptr      = videoPtr;
	int	   cols     = ( x2 >> 2 ) - ( x1 >> 2 ) - 1;
	char	   lmask    = leftPlaneMask [ x1 & 3 ];
	char	   rmask    = rightPlaneMask [ x2 & 3 ];

	if ( cols < 0 )		// both x1 & x2 are located in the same byte
	{
		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, lmask & rmask );

		for ( int y = y1; y <= y2; y++, videoPtr += 80 )
			*videoPtr = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
	}
	else
	{
		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, lmask );

		for ( int y = y1; y <= y2; y++, videoPtr += 80 )
			* videoPtr = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );

		videoPtr = ++ptr;

		for ( y = y1; y <= y2; y++, videoPtr += 80 - cols )
			for ( int x = 0; x < cols; x++ )
				* videoPtr++ = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, rmask );

		videoPtr = ptr + cols;

		for ( y = y1; y <= y2; y++, videoPtr += 80 )
			* videoPtr = color;
	}

	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
}

void	getImage ( int x, int y, int width, int height, void * buf )
{
	char far * videoPtr;
	char	 * dataPtr = (char *) buf;

	for ( int i = x; i < x + width; i++ )
	{
		videoPtr = (char far *) MK_FP ( pageBase, (i >> 2) + y*80 );

		writeReg ( EGA_GRAPHICS, EGA_READ_MAP_SELECT, i & 3 );

		for ( int j = 0; j < height; j++, videoPtr += 80 )
			* dataPtr ++ = * videoPtr;
	}
}

void	putImage ( int x, int y, int width, int height, void * buf )
{
	char far * videoPtr;
	char	 * dataPtr = (char *) buf;

	for ( int i = x; i < x + width; i++ )
	{
		videoPtr = (char far *) MK_FP ( pageBase, (i >> 2) + y*80 );

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 1 << ( i & 3 ) );

		for ( int j = 0; j < height; j++, videoPtr += 80 )
			* videoPtr = * dataPtr++;
	}
}

void	drawString ( int x, int y, char * str, int color )
{
	for ( ; *str != '\0'; str++, x+= 8 )
		for ( int j = 0; j < 16; j++ )
		{
			char	byte = font [16 * (*str) + j];

			for ( int i = 0; i < 8; i++, byte <<= 1 )
				if ( byte & 0x80 )
					writePixel ( x+i, y+j, color );
		}
}

main ()
{
	if ( !findVGA () )
	{
		printf ( "\nVGA compatible card not found." );
		return -1;
	}

	void	* buf = malloc ( 100*50 );

	if ( buf == NULL )
	{
		printf ( "\nmalloc failure." );
		return -1;
	}

	setX ();			// set 320x200 256 colors X-mode

	font = findROMFont ( 16 );

	for ( int i = 0; i < 256; i++ )
		writePixel ( i, 0, i );

	for ( i = 5; i < 140; i++ )
		bar ( 2*i, i, 2*i+30, i+30, i );

	getImage   ( 1, 1, 100, 50, buf );
	drawString ( 110, 100, "Page 0", 70 );
	getch ();

	setActivePage ( 1 );
	setVisualPage ( 1 );
	bar ( 10, 20, 300, 200, 33 );
	drawString ( 110, 100, "Page 1", 75 );
	getch ();

	setActivePage ( 2 );
	setVisualPage ( 2 );
	bar ( 10, 20, 300, 200, 39 );
	drawString ( 110, 100, "Page 2", 80 );
	getch ();

	setActivePage ( 3 );
	setVisualPage ( 3 );
	bar ( 10, 20, 300, 200, 44 );
	drawString ( 110, 100, "Page 3", 85 );
	getch ();

	setVisualPage ( 0 );
	setActivePage ( 0 );
	getch ();
	putImage ( 151, 3, 100, 50, buf );
	getch ();
	setVisualPage ( 1 );
	getch ();
	setVisualPage ( 2 );
	getch ();

	setVideoMode ( 3 );
}
