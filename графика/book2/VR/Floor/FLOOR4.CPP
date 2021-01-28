//
// Drawing textured floor
// with pyramid filtering (mip-mapping)
//
// Author: Alex V. Boreskoff
//
#include	<alloc.h>
#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<mem.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<time.h>
#include	"bmp.h"
#include	"pyramid.h"
#include	"fixmath.h"

#define       ESC  0x011b
#define        UP  0x4800
#define      DOWN  0x5000
#define      LEFT  0x4b00
#define     RIGHT  0x4d00

#define	SCREEN_HEIGHT	200
#define	SCREEN_WIDTH	320

#define	H		(100*256l)
#define	DELTA		1.0		// note : DELTA * NumLines == H
#define	C		0.01
#define	D0		100.0

#define MAKELONG(high, low)	(((long)low) | (((long)high)<<16))

BMPImage     * pic         = new BMPImage ( "FLOOR.BMP" );
BMPImage     * sky         = new BMPImage ( "SKY.BMP" );
ImagePyramid * pyr         = new ImagePyramid ( pic );
char far     * screenPtr   = (char far *) MK_FP ( 0xA000, 0 );
long	       totalFrames = 0;

Fixed	distTable [SCREEN_WIDTH/2];
Fixed	CSinTable [1024];
Fixed	CCosTable [1024];

Fixed	locX, locY;			// viewer loc
Angle	angle;				// viewer angles

int	getShift ( int val )
{
	for ( int s = 0; val != 0; s++, val >>= 1 )
		;

	return s - 1;
}

void	drawSky ()
{
	char far * videoPtr   = screenPtr;
	int	   angleShift = angle >> 7;
	char     * skyPic     = sky -> data;

	for ( int row = 0; row < 100; row++, skyPic += sky -> width )
	{
		for ( int col = 0; col < 320; col++ )
			*videoPtr++ = skyPic [ ( col - angleShift ) & ( sky -> width - 1 ) ];
	}
}

void	drawView ()
{
	char far * videoPtr   = screenPtr + 100*320;
	long	   widthMask  = MAKELONG ( pic -> width - 1, 0xFFFF );
	long	   heightMask = MAKELONG ( pic -> height - 1, 0xFFFF );
	char     * picData    = pic -> data;

	totalFrames++;

	drawSky ();

	for ( int row = 0; row < 100; row++ )
	{
		Fixed	dist = ( H * distTable [row] ) >> 8;
		Fixed	u0   = ( locX + dist * ( cosine ( angle ) >> 8 ) ) & widthMask;
		Fixed	v0   = ( locY + dist * ( sine ( angle ) >> 8 ) ) & heightMask;
		Fixed   du   = ( dist * CSinTable [angle >> 6] ) >> 8;
		Fixed	dv   = ( -dist * CCosTable [angle >> 6] ) >> 8;
		long	widthMask  = MAKELONG ( pic -> width - 1, 0xFFFF );
		long	heightMask = MAKELONG ( pic -> height - 1, 0xFFFF );
		int	widthShift = getShift ( pic -> width );

		int     scale  = dist / (256l*100 );	// Dist * C
		int	level  = 0;			// start with level 0

		while ( scale / 2 > 1 )
		{
			u0         /= 2;
			v0         /= 2;
			du         /= 2;
			dv         /= 2;
			scale      /= 2;
			widthMask  /= 2;
			heightMask /= 2;
			widthShift --;
			level ++;
		}

		Fixed	u    = u0;
		Fixed	v    = v0;

		picData   = pyr -> data + pyr -> offs [level];
		videoPtr += 160;

		for ( int col = 159; col >= 0; col-- )
		{
			* videoPtr-- = picData [fixed2Int ( u ) + ( fixed2Int ( v ) << widthShift )];

			u = ( u - du ) & widthMask;
			v = ( v - dv ) & heightMask;
		}

		videoPtr += 160;

		for ( col = 160, u = u0, v = v0; col < 320; col++ )
		{
			u = ( u + du ) & widthMask;
			v = ( v + dv ) & heightMask;

			* videoPtr++ = picData [ fixed2Int ( u ) + ( fixed2Int ( v ) << widthShift ) ];
		}
	}
}

void	setVideoMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

void	setPalette ( RGB * palette )
{
	for ( int i = 0; i < 256; i++ )		// convert from 8-bit to 6-bit values
	{
		palette [i].red   >>= 2;
		palette [i].green >>= 2;
		palette [i].blue  >>= 2;
	}

	asm {					// really load palette via BIOS
		push	es
		mov	ax, 1012h
		mov	bx, 0			// first color to set
		mov	cx, 256			// # of colors
		les	dx, palette		// ES:DX == table of color values
		int	10h
		pop	es
	}
}

void	initTables ()
{
	initFixMath ();

	for ( int i = 0; i < SCREEN_HEIGHT / 2; i++ )
		distTable [i] = float2Fixed ( D0 / ( ( i + 1 ) * DELTA ) ) >> 8;

	for ( i = 0; i < 1024; i++ )
	{
		float	x = i * 64.0 * M_PI / 32768;

		CSinTable [i] = (long)( C * 65536.0 * sin ( x ) );
		CCosTable [i] = (long)( C * 65536.0 * cos ( x ) );
	}
}

void	draw ( int x, int y, char * data, int w )
{
	for ( int i = 0; i < w; i++ )
		for ( int j = 0; j < w; j++ )
			*(screenPtr + x + i + (y+j)*320) = data [i+j*w];
}

main ()
{
	int	done = 0;

	angle = 0;
	locX  = 0;
	locY  = 0;

	setVideoMode ( 0x13 );
	setPalette   ( pic -> palette );
	initTables   ();

	draw ( 0,   0, pyr -> data, 64 );
	draw ( 70,  0, pyr -> data + pyr -> offs [1], 32 );
	draw ( 110, 0, pyr -> data + pyr -> offs [2], 16 );
	draw ( 130, 0, pyr -> data + pyr -> offs [3], 8 );
	draw ( 150, 0, pyr -> data + pyr -> offs [4], 4 );
	draw ( 160, 0, pyr -> data + pyr -> offs [5], 2 );
	draw ( 170, 0, pyr -> data + pyr -> offs [6], 1 );
	bioskey ( 0 );

	int	start = clock ();

	while ( !done )
	{
		drawView ();

		if ( bioskey ( 1 ) )
		{
			Fixed	vx = cosine ( angle ) * 1000;
			Fixed	vy = sine ( angle ) * 1000;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					angle += ANGLE_90 / 20;
					break;

				case RIGHT:
					angle -= ANGLE_90 / 20;
					break;

				case UP:
					locX += vx;
					locY += vy;
					break;

				case DOWN:
					locX -= vx;
					locY -= vy;
					break;

				case ESC:
					done = 1;
					break;
			}
		}
	}

	float	totalTime = ( clock () - start ) / CLK_TCK;

	setVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %7ld",  totalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", totalTime );
	printf ( "\nFPS                : %7.2f", totalFrames / totalTime );
}
