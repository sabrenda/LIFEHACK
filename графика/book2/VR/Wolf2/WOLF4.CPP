#include	<alloc.h>
#include	<bios.h>
#include	<dos.h>
#include	<math.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	"bmp.h"

#define	SCREEN_WIDTH	320
#define	SCREEN_HEIGHT	200
#define	STEP_BITS	6
#define	STEP		(1 << STEP_BITS)
#define	STEP_MASK	(STEP - 1)
#define	SCALE		4

#define	HORZ_WALL	0
#define	VERT_WALL       1
				// basic colors
#define	FLOOR_COLOR	140
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

BMPImage * pic = new BMPImage ( "WALL.BMP" );

/////////////////////////// Look Up Tables ///////////////////////////

long	SinTable [360];
long	CosTable [360];
int   * j1Table;
int   *	j2Table;
long  *	dhTable;

///////////////////////////////////////////////////////////////////////

void	InitTables ()
{
	int	h;

	for ( int i = 0; i < 360; i++ )
	{
		double	x = i * M_PI / 180;

		SinTable [i] = (long)( SCALE * 65536.0 * sin ( x ) );
		CosTable [i] = (long)( SCALE * 65536.0 * cos ( x ) );
	}

	j1Table = new int  [SCREEN_HEIGHT*64];
	j2Table = new int  [SCREEN_HEIGHT*64];
	dhTable = new long [SCREEN_HEIGHT*64];

	for ( i = 1 ; i < SCREEN_HEIGHT * 64; i++ )
	{
		h           = (int)( (SCREEN_HEIGHT*64l) / i );
		j1Table [i] = ( SCREEN_HEIGHT - h ) >> 1;
		j2Table [i] = j1Table [i] + h;
		dhTable [i] = ( ( 2l << 16 ) * i ) / SCREEN_HEIGHT;
	}
}

void	DrawSpan ( long dist, int t, int x )
{
//return;
	char far * vptr = (char far *) MK_FP ( 0xA000, x );
	long	   dh;
	int	   j1, j2;
	char     * img  = pic -> Data + t;
	long	   y    = 0l;
						// draw ceiling
	if ( dist < SCREEN_HEIGHT * 64 )
	{
		j1 = j1Table [dist];
		j2 = j2Table [dist];
		dh = dhTable [dist];
	}
	else
	{
		j1 = j2 = SCREEN_HEIGHT / 2;
		dh = 0l;
	}

	for ( int j = 0; j < j1; j++, vptr += 320 )
		* vptr = CEILING_COLOR;

	if ( dist < SCREEN_HEIGHT * 64 )
	{
		if ( j2 > SCREEN_HEIGHT )
			j2 = SCREEN_HEIGHT;
						// skip invisible part of wall
		if ( j1 < 0 )
			y = ( (long)( 64 - dist ) ) << 16;

						// draw wall
		for ( ; j < j2; j++, vptr += 320, y += dh )
			* vptr = img [(y>>16)<<7];
	}
						// draw floor
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
	long	t;
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
		{
			d = ( dy << 18 ) / b1;
			t = ( u << 1 ) / b1;
		}
		else
		{
			d = ( dx << 18 ) / a1;
			t = ( v << 1 ) / a1;
		}

		DrawSpan ( d, (int) t, k );

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

void	SetPalette ( RGB far * Palette )
{
	for ( int i = 0; i < 256; i++ )		// convert from 8-bit to 6-bit values
	{
		Palette [i].Red   >>= 2;
		Palette [i].Green >>= 2;
		Palette [i].Blue  >>= 2;
	}

	asm {					// really load palette via BIOS
		push	es
		mov	ax, 1012h
		mov	bx, 0			// first color to set
		mov	cx, 256			// # of colors
		les	dx, Palette		// ES:DX == table of color values
		int	10h
		pop	es
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
	SetPalette ( pic -> Palette );

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
