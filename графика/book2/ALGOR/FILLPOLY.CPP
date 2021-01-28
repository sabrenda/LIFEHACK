#include	<conio.h>
#include	<graphics.h>
#include	<mem.h>
#include	<process.h>
#include	<stdio.h>
#include	"FixMath.h"

struct	Point
{
	int	x;
	int	y;
};

struct	AETEntry
{
	int	from;		// from vertex
	int	to;		// to vertex
	Fixed	x;
	Fixed	dx;
	int	dir;
};

static	AETEntry aet [20];	// active edge table
static	int	 aetCount;	// # of items in AET
static	int	 cr [20];	// list of critical points
static	int	 crCount;	// # of critical points

static	int	findEdge ( int& j, int dir, int n, Point p [] )
{
	for ( ; ; )
	{
		int	j1 = j + dir;

		if ( j1 < 0 )
			j1 = n - 1;
		else
		if ( j1 >= n )
			j1 = 0;

		if ( p [j1].y < p [j].y )	// edge j,j1 is going upwards
			return -1;
		else
		if ( p [j1].y == p [j].y )
			j = j1;
		else
			return j1;
	}
}

void	addEdge ( int j, int j1, int dir, Point p [] )
{
	AETEntry tmp;

	tmp.from = j;
	tmp.to   = j1;
	tmp.x    = Int2Fixed ( p [j].x );
	tmp.dx   = Int2Fixed ( p [j1].x - p [j].x ) / ( p [j1].y - p [j].y );
	tmp.dir  = dir;

	for ( int i = 0; i < aetCount; i++ )	// use x + dr for edges going
						// from the same point
		if ( tmp.x + tmp.dx < aet [i].x + aet [i].dx )
			break;

	// insert tmp at position i
	memmove ( &aet [i+1], &aet [i], (aetCount - i) * sizeof ( AETEntry ) );
	aet [i] = tmp;
	aetCount++;
}

void	buildCR ( int n, Point p [] )
{
	int	candidate = 0;

	crCount = 0;
					// find all critical points
	for ( int i = 0, j = 1; i < n; i++, j++ )
	{
		if ( j >= n )		// check for overflow
			j = 0;

		if ( p [i].y > p [j].y )
			candidate = 1;
		else
		if ( p [i].y < p [j].y && candidate )
		{
			candidate = 0;
			cr [crCount++] = i;
		}
	}

	if ( candidate && p [0].y < p [1].y )
		cr [crCount++] = 0;
					// now sort critical points on y
	for ( i = 0; i < crCount; i++ )
		for ( int j = i + 1; j < crCount; j++ )
			if ( p [cr [i]].y > p [cr [j]].y )
			{		// swap cr [i] and cr [j]
				int	tmp = cr [i];

				cr [i] = cr [j];
				cr [j] = tmp;
			}

}

void	fillPoly ( int n, Point p [] )
{
	int	yMin = p [0].y;
	int	yMax = p [0].y;
	int	k    = 0;

	for ( int i = 1; i < n; i++ )
		if ( p [i].y < yMin )
			yMin = p [i].y;
		else
		if ( p [i].y > yMax )
			yMax = p [i].y;

	buildCR ( n, p );

	aetCount = 0;

	for ( int s = yMin; s <= yMax; s++ )
	{
		for ( ; k < crCount && s == p [cr [k]].y; k++ )
		{
			int	j  = cr [k];
			int	j1 = findEdge ( j, -1, n, p );

			addEdge ( j, j1, -1, p );

			j  = cr [k];
			j1 = findEdge ( j, 1, n, p );

			addEdge ( j, j1, 1, p );
		}

		for ( i = 0; i < aetCount; i += 2 )
			line ( Fixed2Int ( aet [i].x ), s, Fixed2Int ( aet [i+1].x ), s );

		for ( i = 0; i < aetCount; i++ )
		{
			aet [i].x += aet [i].dx;

			if ( p [aet [i].to].y == s )
			{
				int	j  = aet [i].to;
				int	j1 = findEdge ( j, aet [i].dir, n, p );

				if ( j1 >= 0 )		// adjust entry
				{
					aet [i].from = j;
					aet [i].to   = j1;
					aet [i].x    = Int2Fixed ( p [j].x );
					aet [i].dx   = Int2Fixed ( p [j1].x - p [j].x ) / ( p [j1].y - p [j].y );
				}
				else
				{
					aetCount--;
					memmove ( &aet [i], &aet [i+1], (aetCount - i) * sizeof ( AETEntry ) );
					i--;		// to compensate for i++
				}
			}
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

	Point	p [] = { { 100, 100 }, { 150, 300 }, { 90, 200 },
			 { 50, 250 }, { 0, 200 }, { 10, 150 }, { 30, 210 } };

	fillPoly ( 7, p );

	getch ();

	closegraph ();
}
