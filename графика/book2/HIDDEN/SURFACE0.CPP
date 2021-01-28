#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

struct	Point			// screen point
{
	int	x, y;
};

void	plotSurface ( float x1, float y1, float x2, float y2, float (*f)( float, float ), float fMin, float fMax, int n1, int n2 )
{
	Point * curLine  = new Point [n1];
	float	phi = 30*M_PI/180;
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
	float	ay = 10 - 300 * yMin / ( yMax - yMin );
	float	by = -300 / ( yMax - yMin );

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
			line ( curLine [j].x, curLine [j].y, curLine [j + 1].x, curLine [j + 1].y );
	}

	delete curLine;
}


float	f ( float x, float y )
{
	float	r =  x*x + y*y;

//	return 0.5*sin(2*x)*sin(2*y);
	return cos ( 2.2*r ) / ( r + 1 );
//	return sin ( r ) / ( r + 1 );
//	return cos ( r ) / ( r + 1 );
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

	plotSurface ( -2, -2, 2, 2, f, -0.5, 1, 20, 20 );

	getch ();
	closegraph ();
}
