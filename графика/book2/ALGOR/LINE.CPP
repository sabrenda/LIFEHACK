#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

// Bresenhames alg.
void	drawLine ( int x1, int y1, int x2, int y2, int color )
{
	int	dx = abs ( x2 - x1 );
	int	dy = abs ( y2 - y1 );
	int	sx = x2 >= x1 ? 1 : -1;
	int	sy = y2 >= y1 ? 1 : -1;

	if ( dy <= dx )
	{
		int	d  = ( dy << 1 ) - dx;
		int	d1 = dy << 1;
		int	d2 = ( dy - dx ) << 1;

		putpixel ( x1, y1, color );

		for ( int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx )
		{
			if ( d > 0 )
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;

			putpixel ( x, y, color );
		}
	}
	else
	{
		int	d  = ( dx << 1 ) - dy;
		int	d1 = dx << 1;
		int	d2 = ( dx - dy ) << 1;

		putpixel ( x1, y1, color );

		for ( int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy )
		{
			if ( d > 0 )
			{
				d += d2;
				x += sx;
			}
			else
				d += d1;

			putpixel ( x, y, color );
		}
	}
}

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	int	x1 = 501,
		y1 = 100 ,
		x2 = 150,
		y2 = 301;

	drawLine ( x1, y1, x2, y2, WHITE );

	getch ();
	setcolor ( LIGHTBLUE );
	line ( x1, y1, x2, y2 );

	getch ();
	closegraph ();
}
