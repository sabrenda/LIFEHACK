#include	<conio.h>
#include	<graphics.h>
#include	<mem.h>
#include	<process.h>
#include	<stdio.h>
#include	"fixmath.h"
#include	"point.h"

static	int	findEdge ( int& i, int dir, int n, const Point p [] )
{
	for ( ; ; )
	{
		int	i1 = i + dir;

		if ( i1 < 0 )
			i1 = n - 1;
		else
		if ( i1 >= n )
			i1 = 0;

		if ( p [i1].y < p [i].y )	// edge [i,i1] is going upwards
			return -1;  		// must be some error
		else
		if ( p [i1].y == p [i].y )	// horizontal edge
			i = i1;
		else				// edge [i, i1] is going downwords
			return i1;
	}
}

void	fillConvexPoly ( int n, const Point p [] )
{
	int	yMin          = p [0].y;
	int	yMax          = p [0].y;
	int	topPointIndex = 0;
						// find y-range and
	for ( int i = 1; i < n; i++ )		// topPoint index
		if ( p [i].y < p [topPointIndex].y )
			topPointIndex = i;
		else
		if ( p [i].y > yMax )
			yMax = p [i].y;

	yMin = p [topPointIndex].y;

	if ( yMin == yMax )		// degenerate polygon
	{
		int	xMin = p [0].x;
		int	xMax = p [0].x;
					// find it's x-range
		for ( i = 1; i < n; i++ )
			if ( p [i].x < xMin )
				xMin = p [i].x;
			else
			if ( p [i].x > xMax )
				xMax = p [i].x;
					// and fill it
		line ( xMin, yMin, xMax, yMin );

		return;
	}

	int	i1, i1Next;
	int	i2, i2Next;

	i1     = topPointIndex;
	i1Next = findEdge ( i1, -1, n, p );
	i2     = topPointIndex;
	i2Next = findEdge ( i2, 1, n, p );

	Fixed	x1  = int2Fixed ( p [i1].x );
	Fixed	x2  = int2Fixed ( p [i2].x );
	Fixed	dx1 = fraction2Fixed ( p [i1Next].x - p [i1].x, p [i1Next].y - p [i1].y);
	Fixed	dx2 = fraction2Fixed ( p [i2Next].x - p [i2].x, p [i2Next].y - p [i2].y);

	for ( int y = yMin; y <= yMax; y++ )
	{
		line ( fixed2Int ( x1 ), y, fixed2Int ( x2 ), y );

		x1 += dx1;
		x2 += dx2;

		if ( y + 1 == p [i1Next].y )
		{
			i1 = i1Next;			// switch to next edge
			if ( --i1Next < 0 )
				i1Next = n - 1;
							// check for lower
			if ( p [i1].y == p [i1Next].y )	// horizontal part
				break;

			dx1 = fraction2Fixed ( p [i1Next].x - p [i1].x, p [i1Next].y - p [i1].y);
		}

		if ( y + 1 == p [i2Next].y )
		{
			i2 = i2Next;			// switch to next edge
			if ( ++i2Next >= n )
				i2Next = 0;
							// check for lower
			if ( p [i2].y == p [i2Next].y )	// horizontal part
				break;

			dx2 = fraction2Fixed ( p [i2Next].x - p [i2].x, p [i2Next].y - p [i2].y);
		}
	}
}

Point	p1 [] =
{
	{ 100, 100 }, { 120, 170 }, { 100, 200 }, { 50, 150 }
};

Point	p2 [] =
{
	{ 300, 100 }, { 390, 100 }, { 350, 200 }, { 250, 150 }
};

Point	p3 [] =
{
	{ 500, 100 }, { 550, 200 }, { 490, 200 }, { 450, 170 }
};

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


	fillConvexPoly ( 4, p1 );
	fillConvexPoly ( 4, p2 );
	fillConvexPoly ( 4, p3 );

	getch ();

	closegraph ();
}
