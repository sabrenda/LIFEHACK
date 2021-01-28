#include	<conio.h>
#include	<graphics.h>
#include	<stdio.h>
#include	<process.h>
#include	"FixMath.h"

struct	Point
{
	int	x;
	int	y;
};

void	fillTriangle ( Point p [] )
{
	int	iMax = 0;
	int	iMin = 0;
	int	iMid = 0;

	for ( int i = 1; i < 3; i++ )		// find indices of top &
		if ( p [i].y < p [iMin].y )	// bottom vertices
			iMin = i;
		else
		if ( p [i].y > p [iMax].y )
			iMax = i;

	iMid = 3 - iMin - iMax;			// find index of middle item

	Fixed	dx01 = p [iMax].y != p [iMin].y ?
			Int2Fixed ( p [iMax].x - p [iMin].x ) /
			( p [iMax].y - p [iMin].y ) : 0l;

	Fixed	dx02 = p [iMin].y != p [iMid].y ?
			Int2Fixed ( p [iMid].x - p [iMin].x ) /
			( p [iMid].y - p [iMin].y ) : 0l;

	Fixed	dx21 = p [iMid].y != p [iMax].y ?
			Int2Fixed ( p [iMax].x - p [iMid].x ) /
			( p [iMax].y - p [iMid].y ) : 0l;

	Fixed	x1 = Int2Fixed ( p [iMin].x );
	Fixed	x2 = x1;

	for ( i = p [iMin].y; i <= p [iMid].y; i++ )
	{
		line ( Fixed2Int ( x1 ), i, Fixed2Int ( x2 ), i );
		x1 += dx01;
		x2 += dx02;
	}

	for ( i = p [iMid].y + 1; i <= p [iMax].y; i++ )
	{
		x1 += dx01;
		x2 += dx21;
		line ( Fixed2Int ( x1 ), i, Fixed2Int ( x2 ), i );
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

	Point	p [] = { { 100, 100 }, { 150, 200 }, { 70, 140 } };

	fillTriangle ( p );

	getch ();

	closegraph ();
}
