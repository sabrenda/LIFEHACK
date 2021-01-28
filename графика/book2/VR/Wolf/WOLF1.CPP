//
// Simplest ray-casting engine a'la Wolfenstein 3D
// Uses floating-point numbers
//
// Author: Alex V. Boreskoff
//
#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<stdio.h>
#include	<time.h>

#define	VIEW_WIDTH	( M_PI / 3 )	// viewing angle ( 60 degrees )
#define	SCREEN_WIDTH	320		// size of rendering window
#define	SCREEN_HEIGHT	200
					// basic colors
#define	FLOOR_COLOR	8
#define	CEILING_COLOR	3
#define	WALL_COLOR	1
					// angles
#define	ANGLE_90 	M_PI_2		// 90 degrees
#define	ANGLE_180	M_PI		// 180 degrees
#define	ANGLE_270	(M_PI*1.5)	// 270 degrees

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
float	   locX;
float	   locY;
float	   angle;
long	   totalFrames = 0l;
char far * screenPtr   = (char far *) MK_FP ( 0xA000, 0 );
float	   rayAngle [SCREEN_WIDTH];	// angles for rays from main viewing direction

////////////////////////// Functions ////////////////////////////////////

void	drawSpan ( float dist, char wallColor, int x )
{
	char far * vptr = screenPtr + x;
	int	   h    = dist >= 0.5 ? (int)(SCREEN_HEIGHT*0.5 / dist ) : SCREEN_HEIGHT;
	int	   j1   = ( SCREEN_HEIGHT - h ) / 2;
	int	   j2   = j1 + h;

	for ( int j = 0; j < j1; j++, vptr += 320 )
		* vptr = CEILING_COLOR;

	if ( j2 > SCREEN_HEIGHT )
		j2 = SCREEN_HEIGHT;

	for ( ; j < j2; j++, vptr += 320 )
		* vptr = wallColor;

	for ( ; j < SCREEN_HEIGHT; j++, vptr += 320 )
		* vptr = FLOOR_COLOR;
}

float	checkVWalls ( float angle )
{
	int	xTile = (int) locX;	// cell indices
	int	yTile = (int) locY;
	float	xIntercept;		// intercept point
	float	yIntercept;
	float	xStep;			// intercept steps
	float	yStep;
	int	dxTile;

	if ( fabs ( cos ( angle ) ) < 1e-7 )
		return 10000.0;

	if ( angle >= ANGLE_270 || angle <= ANGLE_90 )
	{
		xTile ++;
		xStep      = 1;
		yStep      = tan ( angle );
		xIntercept = xTile;
		dxTile     = 1;
	}
	else
	{
		xTile --;
		xStep      = -1;
		yStep      = -tan ( angle );
		xIntercept = xTile + 1;
		dxTile     = -1;
	}
					// find interception point
	yIntercept = locY + (xIntercept - locX) * tan ( angle );

	for ( ; ; )
	{
		yTile = yIntercept;
		if ( xTile < 0 || xTile > 52 || yTile < 0 || yTile > 9 )
			return 10000.0;

		if ( worldMap [yTile][xTile] != ' ' )
			return ( xIntercept - locX ) / cos ( angle );

		xIntercept += xStep;
		yIntercept += yStep;
		xTile      += dxTile;
	}
}

float	checkHWalls ( float angle )
{
	int	xTile = (int) locX;
	int	yTile = (int) locY;
	float	xIntercept;
	float	yIntercept;
	float	xStep;
	float	yStep;
	int	dyTile;

	if ( fabs ( sin ( angle ) ) < 1e-7 )
		return 10000.0;

	if ( angle <= ANGLE_180 )
	{
		yTile ++;
		xStep = 1 / tan ( angle );
		yStep = 1;
		yIntercept = yTile;
		dyTile = 1;
	}
	else
	{
		yTile --;
		yStep = -1;
		xStep = -1 / tan ( angle );
		yIntercept = yTile + 1;
		dyTile = -1;
	}

	xIntercept = locX + (yIntercept - locY) / tan ( angle );

	for ( ; ; )
	{
		xTile = xIntercept;
		if ( xTile < 0 || xTile > 52 || yTile < 0 || yTile > 9 )
			return 10000.0;

		if ( worldMap [yTile][xTile] != ' ' )
			return ( yIntercept - locY ) / sin ( angle );

		xIntercept += xStep;
		yIntercept += yStep;
		yTile      += dyTile;
	}
}

void	drawView ()
{
	float	phi;
	float	distance;

	totalFrames++;
	for ( int col = 0; col < 320; col++ )
	{
		phi = angle + rayAngle [col];

		if ( phi < 0 )
			phi += 2 * M_PI;
		else
		if ( phi >= 2 * M_PI )
			phi -= 2 * M_PI;

		float	d1 = checkVWalls ( phi );
		float	d2 = checkHWalls ( phi );

		distance = d1;

		if ( d2 < distance )
			distance = d2;

		distance *= cos ( phi - angle );	// adjustment for fish-eye
		drawSpan ( distance, WALL_COLOR, col );
	}
}

void	setVideoMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

void	initTables ()
{
	for ( int i = 0; i < SCREEN_WIDTH; i++ )
		rayAngle [i] = atan ( -swing + 2 * i * swing / ( SCREEN_WIDTH - 1 ) );
}

main ()
{
	int	done = 0;

	angle = 0;
	locX  = 1.5;
	locY  = 1.5;
	swing = tan ( VIEW_WIDTH / 2 );

	initTables ();
	setVideoMode ( 0x13 );

	int	start = clock ();

	while ( !done )
	{
		drawView ();

		if ( bioskey ( 1 ) )
		{
			float	vx = cos ( angle );
			float	vy = sin ( angle );
			float	x, y;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					angle -= 5.0*M_PI/180.0;
					break;

				case RIGHT:
					angle += 5.0*M_PI/180.0;
					break;

				case UP:
					x = locX + 0.3 * vx;
					y = locY + 0.3 * vy;

					if ( worldMap [(int) y][(int) x] == ' ' )
					{
						locX = x;
						locY = y;
					}

					break;

				case DOWN:
					x = locX - 0.3 * vx;
					y = locY - 0.3 * vy;

					if ( worldMap [(int) y][(int) x ] == ' ' )
					{
						locX = x;
						locY = y;
					}

					break;

				case ESC:
					done = 1;
			}

			if ( angle < 0 )
				angle += 2 * M_PI;
			else
			if ( angle >= 2 * M_PI )
				angle -= 2 * M_PI;
		}
	}

	float	totalTime = ( clock () - start ) / CLK_TCK;

	setVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %ld",   totalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", totalTime );
	printf ( "\nFPS                : %7.2f", totalFrames / totalTime );
}