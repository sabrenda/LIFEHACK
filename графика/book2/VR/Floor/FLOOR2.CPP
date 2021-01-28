//
// Simplest way to draw textured floor
// using floating numbers.
// Added drawing the sky a'la DOOM
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

#define	SKY_COLOR	3		// cyan

#define       ESC  0x011b
#define        UP  0x4800
#define      DOWN  0x5000
#define      LEFT  0x4b00
#define     RIGHT  0x4d00

#define	H		100.0
#define	DELTA		1.0		// note : DELAT * NumLines == H
#define	C		0.01
#define	D0		100.0

BMPImage * pic         = new BMPImage ( "FLOOR.BMP" );
BMPImage * sky         = new BMPImage ( "SKY.BMP" );
char far * screenPtr   = (char far *) MK_FP ( 0xA000, 0 );
long	   totalFrames = 0l;

float	x, y;			// viewer loc
float	angle;

int	mod ( float x, int y )
{
	int	res = (int) fmod ( x, y );

	if ( res < 0 )
		res += y;

	return res;
}

void	drawSky ()
{
	char	far * videoPtr = screenPtr;

	for ( int row = 0; row < 100; row++ )
	{
		char * skyPic = sky -> data + row * sky -> width;
		int	angleShift = 512 * angle / M_PI;

		for ( int col = 0; col < 320; col++ )
			*videoPtr++ = skyPic [ ( col - angleShift ) & ( sky -> width - 1 ) ];
	}
}

void	drawView ()
{
	char	far * videoPtr = screenPtr + 100*320;

	totalFrames++;

	drawSky ();

	for ( int row = 0; row < 100; row++ )
	{
		float	dist = H * D0 / ( ( 1 + row ) * DELTA );

		int	i0   = mod ( x + dist * cos ( angle ), pic -> width );
		int	j0   = mod ( y + dist * sin ( angle ), pic -> height );
		float	di   = C * dist * sin ( angle );
		float	dj   = -C * dist * cos ( angle );
		float	ii   = i0;
		float	jj   = j0;
		int	i, j;

		videoPtr += 160;

		for ( int col = 159; col >= 0; col-- )
		{
			i = mod ( ii, pic -> width );
			j = mod ( jj, pic -> height );

			* videoPtr-- = pic -> data [ i + j * pic -> width ];

			ii -= di;
			jj -= dj;
		}

		videoPtr += 160;
		ii = i0 + di;
		jj = j0 + dj;

		for ( col = 160; col < 320; col++ )
		{
			i = mod ( ii, pic -> width );
			j = mod ( jj, pic -> height );

			* videoPtr++ = pic -> data [ i + j * pic -> width ];

			ii += di;
			jj += dj;
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

main ()
{
	int	done  = 0;
	int	start = clock ();

	angle = 0;
	x     = 0;
	y     = 0;

	setVideoMode ( 0x13 );
	setPalette ( pic -> palette );

	while ( !done )
	{
		drawView ();

		if ( bioskey ( 1 ) )
		{
			float	vx = cos ( angle ) * 10;
			float	vy = sin ( angle ) * 10;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					angle += 10 * M_PI / 180;
					break;

				case RIGHT:
					angle -= 10 * M_PI / 180;
					break;

				case UP:
					x += vx;
					y += vy;
					break;

				case DOWN:
					x -= vx;
					y -= vy;
					break;

				case ESC:
					done = 1;
					break;
			}
		}
	}

	float	totalTime = ( clock () - start ) / CLK_TCK;

	setVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %ld",   totalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", totalTime );
	printf ( "\nFPS                : %7.2f", totalFrames / totalTime );
}
