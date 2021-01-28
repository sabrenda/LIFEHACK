#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

#define	NO_VALUE	7777

struct	Point			// screen point
{
	int	x, y;
};

int	YMax [640];
int	YMin [640];
int	upColor   = LIGHTGREEN;
int	downColor = LIGHTGRAY;

void	drawLine ( Point& p1, Point& p2 )
{
	int	dx = abs ( p2.x - p1.x );
	int	dy = abs ( p2.y - p1.y );
	int	sx = p2.x >= p1.x ? 1 : -1;
	int	sy = p2.y >= p1.y ? 1 : -1;

	if ( dy <= dx )
	{
		int	d  = -dx;
		int	d1 = dy << 1;
		int	d2 = ( dy - dx ) << 1;

		for ( int x = p1.x, y = p1.y, i = 0; i <= dx; i++, x += sx )
		{
			if ( YMin [x] == NO_VALUE ) // YMin, YMax not inited
			{
				putpixel ( x, y, upColor );
				YMin [x] = YMax [x] = y;
			}
			else
			if ( y < YMin [x] )
			{
				putpixel ( x, y, upColor );
				YMin [x] = y;
			}
			else
			if ( y > YMax [x] )
			{
				putpixel ( x, y, downColor );
				YMax [x] = y;
			}

			if ( d > 0 )
			{
				d += d2;
				y += sy;
			}
			else
				d += d1;
		}
	}
	else
	{
		int	d  = -dy;
		int	d1 = dx << 1;
		int	d2 = ( dx - dy ) << 1;
		int	m1 = YMin [p1.x];
		int	m2 = YMax [p1.x];

		for ( int x = p1.x, y = p1.y, i = 0; i <= dy; i++, y += sy )
		{
			if ( YMin [x] == NO_VALUE ) // YMin, YMax not inited
			{
				putpixel ( x, y, upColor );
				YMin [x] = YMax [x] = y;
			}
			else
			if ( y < m1 )
			{
				putpixel ( x, y, upColor );
				if ( y < YMin [x] )
					YMin [x] = y;
			}
			else
			if ( y > m2 )
			{
				putpixel ( x, y, downColor );
				if ( y > YMax [x] )
					YMax [x] = y;
			}

			if ( d > 0 )
			{
				d += d2;
				x += sx;
				m1 = YMin [x];
				m2 = YMax [x];
			}
			else
				d += d1;
		}
	}
}

void	plotSurface ( float x1, float y1, float x2, float y2, float (*f)( float, float ), float fMin, float fMax, int n1, int n2 )
{
	Point * curLine  = new Point [n1];
	float   phi = 30*M_PI/180;
	float   psi = 10*M_PI/180;
	float	sphi = sin ( phi );
	float	cphi = cos ( phi );
	float	spsi = sin ( psi );
	float	cpsi = cos ( psi );
	float	e1 [] = { cphi, sphi, 0 };
	float	e2 [] = { spsi*sphi, -spsi*cphi, cpsi };
	float   x, y;
	float	hx = ( x2 - x1 ) / n1;
	float	hy = ( y2 - y1 ) / n2;
	float   xMin = ( e1 [0] >= 0 ? x1 : x2 )  * e1 [0] + ( e1 [1] >= 0 ? y1 : y2 ) * e1 [1];
	float   xMax = ( e1 [0] >= 0 ? x2 : x1 )  * e1 [0] + ( e1 [1] >= 0 ? y2 : y1 ) * e1 [1];
	float   yMin = ( e2 [0] >= 0 ? x1 : x2 )  * e2 [0] + ( e2 [1] >= 0 ? y1 : y2 ) * e2 [1];
	float   yMax = ( e2 [0] >= 0 ? x2 : x1 )  * e2 [0] + ( e2 [1] >= 0 ? y2 : y1 ) * e2 [1];
	int	 i, j, k;

	if ( e2 [2] >= 0 )
	{
		yMin += fMin * e2 [2];
		yMax += fMax * e2 [2];
	}
	else
	{
		yMin += fMax * e2 [2];
		yMax += fMin * e2 [2];
	}

	float	ax = 10 - 600 * xMin / ( xMax - xMin );
	float	bx = 600 / ( xMax - xMin );
	float	ay = 10 - 300 * yMin / ( yMax - yMin );
	float	by = -300 / ( yMax - yMin );

	for ( i = 0; i < sizeof ( YMax ) / sizeof ( int ); i++ )
		YMin [i] = YMax [i] = NO_VALUE;

	for ( i = n2 - 1; i > -1; i-- )
	{
		for ( j = 0; j < n1; j++ )
		{
			x = x1 + j * hx;
			y = y1 + i * hy;
			curLine [j].x = (int)( ax + bx * ( x * e1 [0] + y * e1 [1] ) );
			curLine [j].y = (int)( ay + by * ( x * e2 [0] + y * e2 [1] + f ( x, y ) * e2 [2] ) );
		}

		for ( j = 0; j < n1 - 1; j++ )
			drawLine ( curLine [j], curLine [j + 1] );
	}

	delete curLine;
}

void	plotSurface2 ( float x1, float y1, float x2, float y2, float (*f)( float, float ), float fMin, float fMax, int n1, int n2 )
{
	Point * curLine  = new Point [n1];
	Point * nextLine = new Point [n1];
	float   phi = 30*M_PI/180;
	float   psi = 10*M_PI/180;
	float	sphi = sin ( phi );
	float	cphi = cos ( phi );
	float	spsi = sin ( psi );
	float	cpsi = cos ( psi );
	float	e1 [] = { cphi, sphi, 0 };
	float	e2 [] = { spsi*sphi, -spsi*cphi, cpsi };
	float   x, y;
	float	hx = ( x2 - x1 ) / n1;
	float	hy = ( y2 - y1 ) / n2;
	float   xMin = ( e1 [0] >= 0 ? x1 : x2 )  * e1 [0] + ( e1 [1] >= 0 ? y1 : y2 ) * e1 [1];
	float   xMax = ( e1 [0] >= 0 ? x2 : x1 )  * e1 [0] + ( e1 [1] >= 0 ? y2 : y1 ) * e1 [1];
	float   yMin = ( e2 [0] >= 0 ? x1 : x2 )  * e2 [0] + ( e2 [1] >= 0 ? y1 : y2 ) * e2 [1];
	float   yMax = ( e2 [0] >= 0 ? x2 : x1 )  * e2 [0] + ( e2 [1] >= 0 ? y2 : y1 ) * e2 [1];
	int	i, j, k;

	if ( e2 [2] >= 0 )
	{
		yMin += fMin * e2 [2];
		yMax += fMax * e2 [2];
	}
	else
	{
		yMin += fMax * e2 [2];
		yMax += fMin * e2 [2];
	}

	float	ax = 10 - 600 * xMin / ( xMax - xMin );
	float	bx = 600 / ( xMax - xMin );
	float	ay = 10 - 400 * yMin / ( yMax - yMin );
	float	by = -400 / ( yMax - yMin );

	for ( i = 0; i < sizeof ( YMax ) / sizeof ( int ); i++ )
		YMin [i] = YMax [i] = NO_VALUE;

	for ( i = 0; i < n1; i++ )
	{
		x = x1 + i * hx;
		y = y1 + ( n2 - 1 ) * hy;
		curLine [i].x = (int)( ax + bx * ( x * e1 [0] + y * e1 [1] ) );
		curLine [i].y = (int)( ay + by * ( x * e2 [0] + y * e2 [1] + f ( x, y ) * e2 [2] ) );
	}

	for ( i = n2 - 1; i > -1; i-- )
	{
		for ( j = 0; j < n1 - 1; j++ )
			drawLine ( curLine [j], curLine [j + 1] );

		if ( i > 0 )
			for ( j = 0; j < n1; j++ )
			{
				x = x1 + j * hx;
				y = y1 + ( i - 1 ) * hy;
				nextLine [j].x = (int)( ax + bx * ( x * e1 [0] + y * e1 [1] ) );
				nextLine [j].y = (int)( ay + by * ( x * e2 [0] + y * e2 [1] + f ( x, y ) * e2 [2] ) );

				drawLine ( curLine [j], nextLine [j] );
				curLine [j] = nextLine [j];
			}
	}

	delete curLine;
	delete nextLine;
}

float	f ( float x, float y )
{
	float	r =  x*x + y*y;

	return 0.5*sin(2*x)*sin(2*y);
//	return cos ( 2.2*r ) / ( r + 1 );
//	return sin ( r ) / ( r + 1 );
//	return cos ( r ) / ( r + 1 );
}

float	f2 ( float x, float y )
{
	float	r =  x*x + y*y;

	return cos ( 2.2*r ) / ( r + 1 );
//	return cos ( r ) / ( r + 1 );	// good also
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

//	plotSurface ( -2, -2, 2, 2, f2, -0.5, 1, 40, 40 );
	plotSurface2 ( -2, -2, 2, 2, f2, -0.5, 1, 40, 40 );

	getch ();
	closegraph ();
}
