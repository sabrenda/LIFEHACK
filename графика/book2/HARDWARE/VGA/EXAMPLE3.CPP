#include	<alloc.h>
#include	<conio.h>
#include	<mem.h>
#include	<stdio.h>
#include	"Ega.h"

unsigned   pageBase = 0;
int	   bytesPerLine;
char       leftPlaneMask  [] = { 0x0F, 0x0E, 0x0C, 0x08 };
char	   rightPlaneMask [] = { 0x01, 0x03, 0x07, 0x0F };
char far * font;

void	setX320x240 ()
{
	static int CRTCTable [] = {
		0x0D06,  		// vertical total
		0x3E07,  	        // overflow (bit 8 of vertical counts)
		0x4109,  		// cell height (2 to double-scan)
		0xEA10,  		// vert sync start
		0xAC11,			// vert sync end and protect cr0-cr7
		0xDF12,  		// vertical displayed
		0x0014,  		// turn off dword mode
		0xE715,  		// vert blank start
		0x0616,  		// vert blank end
		0xE317  		// turn on byte mode
	};

	setVideoMode ( 0x13 );

	pageBase     = 0xA000;
	bytesPerLine = 80;
	writeReg ( EGA_SEQUENCER, 4, 6 );
	writeReg ( EGA_CRTC, 0x17, 0xE3 );
	writeReg ( EGA_CRTC, 0x14, 0 );

	writeReg ( EGA_SEQUENCER, 0, 1 );	// synchronous reset
	outportb ( 0x3C2, 0xE3 );		// select 25 MHz dot clock
						// & 60 Hz scan rate
	writeReg ( EGA_SEQUENCER, 0, 3 );	// restart sequencer
	writeReg ( EGA_CRTC, 0x11, ReadReg ( EGA_CRTC, 0x11 ) & 0x7F );

	for ( int i = 0; i < sizeof ( CRTCTable ) / sizeof ( int ); i++ )
		outport ( EGA_CRTC, CRTCTable [i] );

						// clear screen
	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
	_fmemset ( MK_FP ( pageBase, 0 ), '\0', 0xFFFF );
}

void	setX360x480 ()
{
	static int CRTCTable [] = {
		0x6b00,
		0x5901,
		0x5A02,
		0x8E03,
		0x5E04,
		0x8A05,
		0x0D06,  	// vertical total
		0x3E07,  	// overflow (bit 8 of vertical counts)
		0x4009,  	// cell height (2 to double-scan)
		0xEA10,  	// vert sync start
		0xAC11,  	// vert sync end and protect cr0-cr7
		0xDF12,  	// vertical displayed
		0x2D13,
		0x0014,  	// turn off dword mode
		0xE715,  	// vert blank start
		0x0616,  	// vert blank end
		0xE317  	// turn on byte mode
	};

	setVideoMode ( 0x13 );

	pageBase     = 0xA000;
	bytesPerLine = 90;

	writeReg ( EGA_SEQUENCER, 4, 6 );
	writeReg ( EGA_CRTC, 0x17, 0xE3 );
	writeReg ( EGA_CRTC, 0x14, 0 );

	writeReg ( EGA_SEQUENCER, 0, 1 );	// synchronous reset
	outportb ( 0x3C2, 0xE7 );		// select 25 MHz dot clock
						// & 60 Hz scan rate
	writeReg ( EGA_SEQUENCER, 0, 3 );	// restart sequencer
	writeReg ( EGA_CRTC, 0x11, ReadReg ( EGA_CRTC, 0x11 ) & 0x7F );

	for ( int i = 0; i < sizeof ( CRTCTable ) / sizeof ( int ); i++ )
		outport ( EGA_CRTC, CRTCTable [i] );

						// clear screen
	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
	_fmemset ( MK_FP ( pageBase, 0 ), '\0', 0xFFFF );
}

void	setVisualPage ( int page )
{
	unsigned addr = page * 0x4B00;

				// wait for vertical retrace
	while ( ( inportb ( 0x3DA ) & 0x08 ) == 0 )
		;

	writeReg ( EGA_CRTC, 0x0C, addr >> 8 );
	writeReg ( EGA_CRTC, 0xDC, addr & 0x0F );
}

void	setActivePage ( int page )
{
	pageBase = 0xA000 + page * 0x4B0;
}

void	writePixel ( int x, int y, int color )
{
	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 1 << ( x & 3 ) );

	pokeb ( pageBase, y * bytesPerLine + ( x >> 2 ), color );

	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
}

int	readPixel ( int x, int y )
{
	writeReg ( EGA_GRAPHICS, EGA_READ_MAP_SELECT, x & 3 );

	return peekb ( pageBase, y * bytesPerLine + ( x >> 2 ) );
}

void	bar ( int x1, int y1, int x2, int y2, int color )
{
	char far * vptr  = (char far *) MK_FP ( pageBase, y1 * bytesPerLine + (x1 >> 2) );
	char far * ptr   = vptr;
	int	   cols  = ( x2 >> 2 ) - ( x1 >> 2 ) - 1;
	char	   lmask = leftPlaneMask [ x1 & 3 ];
	char	   rmask = rightPlaneMask [ x2 & 3 ];

	if ( cols < 0 )		// both x1 & x2 are located in the same byte
	{
		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, lmask & rmask );

		for ( int y = y1; y <= y2; y++, vptr += bytesPerLine )
			*vptr = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
	}
	else
	{
		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, lmask );

		for ( int y = y1; y <= y2; y++, vptr += bytesPerLine )
			*vptr = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );

		vptr = ++ptr;

		for ( y = y1; y <= y2; y++, vptr += bytesPerLine - cols )
			for ( int x = 0; x < cols; x++ )
				*vptr++ = color;

		writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, rmask );

		vptr = ptr + cols;

		for ( y = y1; y <= y2; y++, vptr += bytesPerLine )
			*vptr = color;
	}

	writeReg ( EGA_SEQUENCER, EGA_MAP_MASK, 0x0F );
}

void	drawString ( int x, int y, char * str, int color )
{
	for ( ; *str != '\0'; str++, x+= 8 )
		for ( int j = 0; j < 16; j++ )
		{
			char	byte = font [16 * (*str) + j];

			for ( int i = 0; i < 8; i++, byte <<= 1 )
				if ( byte & 0x80 )
					dritePixel ( x+i, y+j, color );
		}
}

void	show360x480 ()
{
	RGB	pal [256];

	setX360x480 ();			// set 320x480 256 colors X-mode

	for ( int i = 0; i < 256; i++ )
	{
		int	c = i & 0x40 ? i & 0x3F : 0x3F - ( i & 0x3F );

		pal [i].red   = c;
		pal [i].green = c * c / 0x3F;
		pal [i].blue  = i & 0x80 ? 0x3F - ( i >> 1 ) & 0x3F : ( i >> 1 ) & 0x3F;
	}

	setPalette ( pal, 256 );

	for ( int x = 0; x < 180; x++ )
		for ( int y = 0; y < 240; y++ )
		{
			unsigned long	x2    = ( x + 1 ) * (long)( 360 - x );
			unsigned long	y2    = ( y + 1 ) * (long)( 480 - y );
			int	color = (int)((x2*x2)/y2/113);

			writePixel ( x,       y,       color );
			writePixel ( 359 - x, y,       color );
			writePixel ( x,       479 - y, color );
			writePixel ( 359 - x, 479 - y, color );
		}
}

main ()
{
	if ( !findVGA () )
	{
		printf ( "\nVGA compatible card not found." );
		return -1;
	}

	setX320x240 ();			// set 320x240 256 colors X-mode

	font = findROMFont ( 16 );

	for ( int j = 1; j < 220; j += 21 )
		for ( int i = 1; i < 300; i += 21 )
			bar ( i, j, i + 20, j + 20, ( ( j / 21 * 15 ) + i / 21) & 0xFF );

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

	setVisualPage ( 0 );
	getch ();
	setVisualPage ( 1 );
	getch ();
	setVisualPage ( 2 );
	getch ();

	show360x480 ();

	getch ();
	setVideoMode ( 3 );
}
