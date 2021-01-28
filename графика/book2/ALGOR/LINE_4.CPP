#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

#define	putpixel	DrawPixel

void	drawPixel ( int x, int y, int color )
{
	fillellipse ( 10 * x, 10 * y, 5, 5 );
}

// simplest Bresenhames alg. 0 <= y2 - y1 <= x2 - x1
void	drawLine ( int x1, int y1, int x2, int y2, int color )
{
	int	dx = x2 - x1;
	int	dy = y2 - y1;
	int	d  = ( dy << 1 ) - dx;
	int	d1 = dy << 1;
	int	d2 = ( dy - dx ) << 1;

	putpixel ( x1, y1, color );

	for ( int x = x1 + 1, y = y1; x <= x2; x++ )
	{
		if ( d > 0 )
		{
			d += d2;
			y += 1;
		}
		else
			d += d1;

		putpixel ( x, y, color );
	}
}

void	drawLine4 ( int x1, int y1, int x2, int y2, int color )
{
	int	dx = x2 - x1;
	int	dy = y2 - y1;
	int	d  = 0;			//( dy << 1 ) - dx;
	int	d1 = dy << 1;
	int	d2 = - ( dx << 1 );

	putpixel ( x1, y1, color );

	for ( int x = x1, y = y1, i = 1; i <= dx + dy; i++ )
	{
		if ( d > 0 )
		{
			d += d2;
			y += 1;
		}
		else
		{
			d += d1;
			x += 1;
		}

		putpixel ( x, y, color );
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

	int	x1 = 1,
		y1 = 1 ,
		x2 = 20,
		y2 = 7;

	drawLine  ( x1, y1, x2, y2, WHITE );
	drawLine4 ( x1 + 30, y1, x2 + 30, y2, YELLOW );

	getch ();
	closegraph ();
}
