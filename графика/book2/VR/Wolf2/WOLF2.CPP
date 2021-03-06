#include	<alloc.h>
#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>

#define	SCREEN_WIDTH	320
#define	SCREEN_HEIGHT	200
#define	STEP_BITS	6
#define	STEP		(1 << STEP_BITS)
#define	STEP_MASK	(STEP - 1)
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

char  * Map [] = {
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

long	   Swing;
long	   LocX;
long	   LocY;
int	   Angle;
long	   TotalFrames = 0l;
long far * TicksPtr = ( long far * ) 0x46C;

long	SinTable [360];
long	CosTable [360];

void	InitTables ()
{
	for ( int i = 0; i < 360; i++ )
	{
		double	x = i * M_PI / 180;

                SinTable [i] = (long)( SCALE * 65536.0 * sin ( x ) );
		CosTable [i] = (long)( SCALE * 65536.0 * cos ( x ) );
	}
}

void	DrawSpan ( long dist, char WallColor, int x )
{
//return;
	char far * vptr = (char far *) MK_FP ( 0xA000, x );
	int	   h    = dist >= 64l ? (int)( (SCREEN_HEIGHT*64l) / dist ) : SCREEN_HEIGHT;
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

void	DrawView ()
{
	int	i0 = (int)( LocX >> STEP_BITS );
	int	j0 = (int)( LocY >> STEP_BITS );
	long	u0 = LocX & STEP_MASK;
	long	v0 = LocY & STEP_MASK;
	long	vx = CosTable [Angle];
	long	vy = SinTable [Angle];
	long	nx = vy;
	long	ny = -vx;
	long	da = ( ( nx >> 7 ) * ( Swing >> 8 ) ) / SCREEN_WIDTH;
	long	db = ( ( ny >> 7 ) * ( Swing >> 8 ) ) / SCREEN_WIDTH;
	long	a  = vx - ( Swing >> 8 ) * ( nx >> 8 );
	long	b  = vy - ( Swing >> 8 ) * ( ny >> 8 );
	long	u, v, u1, v1;
	long	a1, b1;
	long	d, dx, dy;
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
		u1 = b1 << STEP_BITS;
		v1 = a1 << STEP_BITS;
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
			d = ( dy << 18 ) / b1;
		else
			d = ( dx << 18 ) / a1;

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
	LocX  = (long)(STEP * 1.3 );
	LocY  = (long)(STEP * 1.3 );
	Swing = (long)(tan ( 30.0 * M_PI / 180.0 ) * 65536.0 );

	InitTables ();
	SetVideoMode ( 0x13 );

	long	Ticks = * TicksPtr;

	while ( !Done )
	{
		DrawView ();

		if ( bioskey ( 1 ) )
		{
			long	vx = CosTable [Angle] >> 14;
			long	vy = SinTable [Angle] >> 14;
			long	x, y;

			switch ( bioskey ( 0 ) )
			{
				case LEFT:
					Angle += 5;
					break;

				case RIGHT:
					Angle -= 5;
					break;

				case UP:
					x = LocX + vx;
					y = LocY + vy;

					if ( Map [(int) ( x >> STEP_BITS )][(int)( y >> STEP_BITS )] == ' ' )
					{
						LocX = x;
						LocY = y;
					}

					break;

				case DOWN:
					x = LocX - vx;
					y = LocY - vy;

					if ( Map [(int) ( x >> STEP_BITS )][(int)( y >> STEP_BITS )] == ' ' )
					{
						LocX = x;
						LocY = y;
					}

					break;

				case ESC:
					Done = 1;
					break;
			}

			if ( Angle < 0 )
				Angle += 360;
			else
			if ( Angle >= 360 )
				Angle -= 360;
		}
	}

	long	TotalTicks = labs ( * TicksPtr - Ticks );

	SetVideoMode ( 0x03 );

	printf ( "\nFrames rendered    : %ld",   TotalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", TotalTicks * 0.055 );
	printf ( "\nFPS                : %7.2f", TotalFrames / ( TotalTicks * 0.055 ) );
}