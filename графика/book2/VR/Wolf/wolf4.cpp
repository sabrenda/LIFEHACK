//
// Simplest ray-casting engine a'la Wolfenstein 3D
// Uses fixed-point numbers
// Draws textures on walls
//
// Author: Alex V. Boreskoff
//

#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<stdio.h>
#include	<time.h>
#include	"fixmath.h"
#include	"bmp.h"

#define	VIEW_WIDTH	(M_PI / 3)	// viewing angle ( 60 degrees )
#define	SCREEN_WIDTH	320		// size of rendering window
#define	SCREEN_HEIGHT	200
					// basic colors
#define	FLOOR_COLOR	140
#define	CEILING_COLOR	3
					// bios key defintions
#define       ESC  0x011b
#define        UP  0x4800
#define      DOWN  0x5000
#define      LEFT  0x4b00
#define     RIGHT  0x4d00
					// labyrinth
char  * worldMap [] =
{
	"****************************************************",
	"*                       *                          *",
	"*   * * * * * * * * *   ************************** *",
	"*                       *                          *",
	"*   * * * * * * * * *   * ************************ *",
	"*                                                * *",
	"*   * * * * * * * * *   ************************** *",
	"*                       *                          *",
	"****************************************************"
};

float	   swing;
Fixed	   locX;
Fixed	   locY;
Angle	   angle;
Fixed	   xIntV;			// interception point for vertical walls
Fixed	   yIntV;
Fixed	   xIntH;			// interception point for horizontal walls
Fixed      yIntH;
long	   totalFrames = 0l;
char far * screenPtr   = (char far *) MK_FP ( 0xA000, 0 );
BMPImage * pic         = new BMPImage ( "WALL.BMP" );
Angle	   rayAngle [SCREEN_WIDTH];	// angles for rays from main viewing direction

////////////////////////// Functions ////////////////////////////////////

void	drawSpan ( Fixed dist, int textOffs, int x )
{
	char far * vptr = screenPtr + x;
	int	   h    = dist >= (ONE / 2) ? fixed2Int ((int2Fixed (SCREEN_HEIGHT/2) << 8) / (dist >> 8)) : SCREEN_HEIGHT;
	int	   j1   = ( SCREEN_HEIGHT - h ) / 2;
	int	   j2   = j1 + h;
	char     * img  = pic -> data + textOffs;
	Fixed	   y    = 0l;
	Fixed	   dy   = (2 * 128 * dist) / SCREEN_HEIGHT;
						// draw ceiling
	for ( register int j = 0; j < j1; j++, vptr += 320 )
		* vptr = CEILING_COLOR;

	if ( j2 > SCREEN_HEIGHT )
		j2 = SCREEN_HEIGHT;
						// skip invisible part of wall
	for ( j = j1; j < 0; j++, y += dy )
		;
						// draw wall
						// Note: a << 7 == a * 128
						// where bmp -> width == 128
	for ( ; j < j2; j++, vptr += 320, y += dy )
		* vptr = img [fixed2Int(y)<<7];
						// draw floor
	for ( ; j < SCREEN_HEIGHT; j++, vptr += 320 )
		* vptr = FLOOR_COLOR;
}

Fixed	checkVWalls ( Angle angle )
{
	int	xTile = fixed2Int ( locX );	// cell indices
	int	yTile = fixed2Int ( locY );
	Fixed	xIntercept;			// intercept point
	Fixed	yIntercept;
	Fixed	xStep;				// intercept steps
	Fixed	yStep;
	int	dxTile;

	if ( fixAbs ( coTang ( angle ) ) < 2 )
		return MAX_FIXED;

	if ( angle >= ANGLE_270 || angle <= ANGLE_90 )
	{
		xTile ++;
		xStep      = ONE;
		yStep      = tang      (angle);
		xIntercept = int2Fixed (xTile);
		dxTile     = 1;
	}
	else
	{
		xTile --;
		xStep      = -ONE;
		yStep      = -tang      (angle);
		xIntercept =  int2Fixed (xTile + 1);
		dxTile     = -1;
	}
					// find interception point
	yIntercept = locY + ((xIntercept - locX) << 15) / (coTang (angle) >> 1);

	for ( ; ; )
	{
		yTile = fixed2Int (yIntercept);
		if ( xTile < 0 || xTile > 52 || yTile < 0 || yTile > 9 )
			return MAX_FIXED;

		if ( worldMap [yTile][xTile] != ' ' )
		{
			yIntV = yIntercept;	// store interception point for
			xIntV = xIntercept;	// computing texture offset

			return ((xIntercept - locX) >> 8) * (invCosine ( angle ) >> 8);
		}

		xIntercept += xStep;
		yIntercept += yStep;
		xTile      += dxTile;
	}
}

Fixed	checkHWalls ( Angle angle )
{
	Fixed	xTile = fixed2Int ( locX );
	Fixed	yTile = fixed2Int ( locY );
	Fixed	xIntercept;
	Fixed	yIntercept;
	Fixed	xStep;
	Fixed	yStep;
	int	dyTile;

	if ( fixAbs ( tang ( angle ) ) < 2 )
		return MAX_FIXED;

	if ( angle <= ANGLE_180 )
	{
		yTile ++;
		xStep      = coTang (angle);
		yStep      = ONE;
		yIntercept = int2Fixed (yTile);
		dyTile     = 1;
	}
	else
	{
		yTile --;
		yStep      = -ONE;
		xStep      = -coTang ( angle );
		yIntercept = int2Fixed (yTile + 1);
		dyTile     = -1;
	}

	xIntercept = locX + ((yIntercept - locY) << 15) / (tang (angle) >> 1);

	for ( ; ; )
	{
		xTile = fixed2Int (xIntercept);
		if ( xTile < 0 || xTile > 52 || yTile < 0 || yTile > 9 )
			return MAX_FIXED;

		if ( worldMap [yTile][xTile] != ' ' )
		{
			xIntH = xIntercept;		// store interception point
			yIntH = yIntercept;		// for computing texture offset

			return ((yIntercept - locY) >> 8) * (invSine ( angle ) >> 8);
		}

		xIntercept += xStep;
		yIntercept += yStep;
		yTile      += dyTile;
	}
}

void	drawView ()
{
	Angle	phi;
	Fixed	distance;

	totalFrames++;
	for ( int col = 0; col < 320; col++ )
	{
		phi = angle + rayAngle [col];

		Fixed	d1 = checkVWalls ( phi );
		Fixed	d2 = checkHWalls ( phi );
		int	textureOffset;

		distance = d1;

		if ( d2 < distance )
		{
			distance      = d2;
			textureOffset = fixed2Int ( frac ( xIntH ) << 7 );

			if ( yIntH < locY )
				textureOffset = 127 - textureOffset;
		}
		else
		{
			textureOffset = fixed2Int ( frac ( yIntV ) << 7 );

			if ( xIntV < locX )
				textureOffset = 127 - textureOffset;
		}
						// adjustment for fish-eye
		distance = (distance >> 8) * (cosine ( phi - angle ) >> 8);

		drawSpan ( distance, textureOffset, col );
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

	for ( int i = 0; i < SCREEN_WIDTH; i++ )
		rayAngle [i] = rad2Angle ( atan ( -swing + 2 * i * swing / (SCREEN_WIDTH - 1) ) );
}

main ()
{
	int	done = 0;
	Fixed	ct   = float2Fixed ( 0.3 );
	Angle	da   = 5 * (ANGLE_90 / 90);

	angle = 0;
	locX  = float2Fixed ( 1.5 );
	locY  = float2Fixed ( 1.5 );
	swing = tan ( VIEW_WIDTH / 2 );
/*
locX = 369396l;
locY = 351156l;
angle = 43600U;
*/
/*
locX = 213672;
locY = 108336;
angle = 5500;
*/
locX = 375476;
locY = 358224;
angle = 60084U;

	initTables   ();
	setVideoMode ( 0x13 );
	setPalette   ( pic -> palette );

	int	start = clock ();

	while ( !done )
	{
		drawView ();

		if ( bioskey ( 1 ) )
		{
			Fixed	vx = cosine ( angle );
			Fixed	vy = sine   ( angle );
			Fixed	x, y;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					angle -= da;
					break;

				case RIGHT:
					angle += da;
					break;

				case UP:
					x = locX + (ct >> 8) * (vx >> 8);
					y = locY + (ct >> 8) * (vy >> 8);

					if ( worldMap [fixed2Int (y)][fixed2Int (x)] == ' ' )
					{
						locX = x;
						locY = y;
					}

					break;

				case DOWN:
					x = locX - (ct >> 8) * (vx >> 8);
					y = locY - (ct >> 8) * (vy >> 8);

					if ( worldMap [fixed2Int (y)][fixed2Int (x)] == ' ' )
					{
						locX = x;
						locY = y;
					}

					break;

				case ESC:
					done = 1;
			}
		}
	}

	float	totalTime = ( clock () - start ) / CLK_TCK;

	setVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %ld",   totalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", totalTime );
	printf ( "\nFPS                : %7.2f", totalFrames / totalTime );
}