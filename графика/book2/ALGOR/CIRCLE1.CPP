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

void	circle1 ( int xc, int yc, int r, int color )
{
	int	x = 0;
	int	y = r;
	float	d = 1.25 - r;

	xCenter = xc;
	yCenter = yc;

	circlePoints ( x, y, color );

	while ( y > x )
	{
		if ( d < 0 )
		{
			d += 2*x + 3;
			x++;
		}
		else
		{
			d += 2*(x - y ) + 5;
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

	circle1 ( 320, 200, 140, RED );
	circle1 ( 260, 200, 40, BLUE );
	circle1 ( 380, 200, 40, YELLOW );

	getch ();

	closegraph ();
}
