#include	<alloc.h>
#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>

#define	SCREEN_WIDTH	320
#define	SCREEN_HEIGHT	200
#define	STEP		64
#define	SCALE		3

#define	HORZ_WALL	0
#define	VERT_WALL       1
				// basic colors
#define	FLOOR_COLOR	8
#define	CEILING_COLOR	3
				// bios key defintions
#define       ESC  0x011b
#define        UP  0x4800
#define      DOWN  0x5000
#define      LEFT  0x4b00
#define     RIGHT  0x4d00

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
int	   angle;
long	   totalFrames = 0l;
long far * TicksPtr = ( long far * ) 0x46C;

void	drawSpan ( float dist, char wallColor, int x )
{
	char far * vptr = (char far *) MK_FP ( 0xA000, x );
	int	   h    = dist >= 32 ? (int)(SCREEN_HEIGHT*32 / dist ) : SCREEN_HEIGHT;
	int	   j1   = ( SCREEN_HEIGHT - h ) / 2;
	int	   j2   = j1 + h;

	for ( int j = 0; j < j1; j++, vptr += 320 )
		* vptr = CEILING_COLOR;

	if ( j2 > SCREEN_HEIGHT )
		j2 = SCREEN_HEIGHT;

	for ( ; j < j2; j++, vptr += 320 )
		* vptr = WallColor;

	for ( ; j < SCREEN_HEIGHT; j++, vptr += 320 )
		* vptr = FLOOR_COLOR;
}

void	drawView ()
{
	int	i0 = (int)( locX / STEP );
	int	j0 = (int)( locY / STEP );
	float	u0 = fmod ( locX, STEP );
	float	v0 = fmod ( locY, STEP );
	float	vx = SCALE * cos ( Angle * M_PI / 180 );
	float	vy = SCALE * sin ( Angle * M_PI / 180 );
	float	nx = SCALE * cos ( ( Angle - 90 ) * M_PI / 180 );
	float	ny = SCALE * sin ( ( Angle - 90 ) * M_PI / 180 );
	float	da = 2 * nx * Swing / SCREEN_WIDTH;
	float	db = 2 * ny * Swing / SCREEN_WIDTH;
	float	a  = vx - Swing * nx;
	float	b  = vy - Swing * ny;
	float	u, v, u1, v1;
	float	a1, b1;
	float	d, dx, dy;
	int	i, j;
	int	di, dj;
	int	wall;

	TotalFrames++;

	for ( int k = 0; k < SCREEN_WIDTH; k++ )
	{
		if ( a > 0 )
		{
			di = 1;
			u  = STEP - u0;
			a1 = a;
		}
		else
		{
			di = -1;
			u  = u0;
			a1 = -a;
		}

		if ( b > 0 )
		{
			dj = 1;
			v  = STEP - v0;
			b1 = b;
		}
		else
		{
			dj = -1;
			v  = v0;
			b1 = -b;
		}

		dx = u - STEP;
		dy = v - STEP;
		u *= b1;
		v *= a1;
		u1 = b1*STEP;
		v1 = a1*STEP;
		i  = i0;
		j  = j0;

		while ( Map [i][j] == ' ' )
			if ( u >= v )
			{
				dy  += STEP;
				u   -= v;
				v    = v1;
				j   += dj;
				wall = HORZ_WALL;
			}
			else
			{
				dx  += STEP;
				v   -= u;
				u    = u1;
				i   += di;
				wall = VERT_WALL;
			}

		if ( wall == HORZ_WALL )
			d = dy / b1;
		else
			d = dx / a1;

		DrawSpan ( d, 1, k );

		a += da;
		b += db;
	}
}

void	SetVideoMode ( int mode )
{
	asm {
		mov	ax, mode
		int	10h
	}
}

main ()
{
	int	Done = 0;

	Angle = 15;
	LocX  = STEP * 1.3;
	LocY  = STEP * 1.3;
	Swing = tan ( 30.0 * M_PI / 180.0 );

	SetVideoMode ( 0x13 );

	long	Ticks = * TicksPtr;

	while ( !Done )
	{
		DrawView ();

		if ( bioskey ( 1 ) )
		{
			float	vx = cos ( Angle * M_PI / 180 );
			float	vy = sin ( Angle * M_PI / 180 );
			float	x, y;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					Angle += 5;
					break;

				case RIGHT:
					Angle -= 5;
					break;

				case UP:
					x = LocX + 16 * vx;
					y = LocY + 16 * vy;

					if ( Map [(int) ( x / STEP )][(int)( y / STEP )] == ' ' )
					{
						LocX = x;
						LocY = y;
					}

					break;

				case DOWN:
					x = LocX - 16 * vx;
					y = LocY - 16 * vy;

					if ( Map [(int) ( x / STEP )][(int)( y / STEP )] == ' ' )
					{
						LocX = x;
						LocY = y;
					}

					break;

				case ESC:
					Done = 1;
			}
		}
	}

	long	TotalTicks = labs ( * TicksPtr - Ticks );

	SetVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %ld",   TotalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", TotalTicks * 0.055 );
	printf ( "\nFPS                : %7.2f", TotalFrames / ( TotalTicks * 0.055 ) );
}