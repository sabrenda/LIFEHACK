#include	<conio.h>
#include	<graphics.h>
#include	<stdio.h>
#include	<process.h>

static	int	xCenter;
static	int	yCenter;

static	void	circlePoints ( int x, int y, int color )
{
	putpixel ( xCenter + x, yCenter + y, color );
	putpixel ( xCenter + y, yCenter + x, color );
	putpixel ( xCenter + y, yCenter - x, color );
	putpixel ( xCenter + x, yCenter - y, color );
	putpixel ( xCenter - x, yCenter - y, color );
	putpixel ( xCenter - y, yCenter - x, color );
	putpixel ( xCenter - y, yCenter + x, color );
	putpixel ( xCenter - x, yCenter + y, color );
}

void	circle2 ( int xc, int yc, int r, int color )
{
	int	x      = 0;
	int	y      = r;
	int	d      = 1 - r;
	int	delta1 = 3;
	int	delta2 = -2*r + 5;

	xCenter = xc;
	yCenter = yc;

	circlePoints ( x, y, color );

	while ( y > x )
	{
		if ( d < 0 )
		{
			d      += delta1;
			delta1 += 2;
			delta2 += 2;
			x++;
		}
		else
		{
			d      += delta2;
			delta1 += 2;
			delta2 += 4;
			x++;
			y--;
		}

		circlePoints ( x, y, color );
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

	circle2 ( 320, 200, 140, RED );
	circle2 ( 260, 200, 40, BLUE );
	circle2 ( 380, 200, 40, YELLOW );

	getch ();

	closegraph ();
}
