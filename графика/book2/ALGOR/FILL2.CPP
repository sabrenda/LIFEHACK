#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

int	borderColor = WHITE;
int	color       = GREEN;

int	lineFill ( int x, int y, int dir, int PrevXl, int PrevXr )
{
	int	xl = x;
	int	xr = x;
	int	c;

				// find line segment
	do
		c = getpixel ( --xl, y );
	while ( ( c != borderColor ) && ( c != color ) );

	do
		c = getpixel ( ++xr, y );
	while ( ( c != borderColor ) && ( c != color ) );

	xl++;
	xr--;

	line ( xl, y, xr, y );	// fill segment
				// fill adjacent segments in the same direction
	for ( x = xl; x<= xr; x++ )
	{
		c = getpixel ( x, y + dir );
		if ( ( c != borderColor ) && ( c != color ) )
			x = lineFill ( x, y + dir, dir, xl, xr );
	}

	for ( x = xl; x < PrevXl; x++ )
	{
		c = getpixel ( x, y - dir );
		if ( ( c != borderColor ) && ( c != color ) )
			x = lineFill ( x, y - dir, -dir, xl, xr );
	}

	for ( x = PrevXr; x < xr; x++ )
	{
		c = getpixel ( x, y - dir );
		if ( ( c != borderColor ) && ( c != color ) )
			x = lineFill ( x, y - dir, -dir, xl, xr );
	}

	return xr;
}

void	fill ( int x, int y )
{
	lineFill ( x, y, 1, x, x );
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

	circle ( 320, 200, 140 );
	circle ( 260, 200, 40 );
	circle ( 380, 200, 40 );

	getch ();

	setcolor ( color );

	fill ( 320, 300 );

	getch ();
	closegraph ();
}
