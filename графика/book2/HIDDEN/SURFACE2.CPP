#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>

#include	"Vector3D.h"

struct	Point			// screen point
{
	int	x, y;
};

void	plotShadedSurface ( float x1, float y1, float x2, float y2, float (*f)( float, float ), float fmin, float fmax, int n1, int n2 )
{
	Point    * curLine  = new Point    [n1];
	Point    * nextLine = new Point    [n1];
	Vector3D * curPoint = new Vector3D [n1];
	Vector3D * nextPoint= new Vector3D [n1];
	float      phi      = 30*M_PI/180;
	float      psi      = 20*M_PI/180;
	float	   sphi     = sin ( phi );
	float	   cphi     = cos ( phi );
	float	   spsi     = sin ( psi );
	float	   cpsi     = cos ( psi );
	Vector3D   e1 ( cphi, sphi, 0 );
	Vector3D   e2 ( spsi*sphi, -spsi*cphi, cpsi );
	Vector3D   e3 ( sphi*cpsi, -cphi*cpsi, -spsi );
	float      xMin     = ( e1 [0] >= 0 ? x1 : x2 )  * e1 [0] + ( e1 [1] >= 0 ? y1 : y2 ) * e1 [1];
	float      xMax     = ( e1 [0] >= 0 ? x2 : x1 )  * e1 [0] + ( e1 [1] >= 0 ? y2 : y1 ) * e1 [1];
	float      yMin     = ( e2 [0] >= 0 ? x1 : x2 )  * e2 [0] + ( e2 [1] >= 0 ? y1 : y2 ) * e2 [1];
	float      yMax     = ( e2 [0] >= 0 ? x2 : x1 )  * e2 [0] + ( e2 [1] >= 0 ? y2 : y1 ) * e2 [1];
	float	   hx       = ( x2 - x1 ) / n1;
	float	   hy       = ( y2 - y1 ) / n2;
	Vector3D   edge1, edge2, n;
	Point	   facet [3];
	float      x, y;
	int	   color;
	int	   i, j, k;

	if ( e2 [2] >= 0 )
	{
		yMin += fmin * e2 [2];
		yMax += fmax * e2 [2];
	}
	else
	{
		yMin += fmax * e2 [2];
		yMax += fmin * e2 [2];
	}

	float	ax = 20 - 600 * xMin / ( xMax - xMin );
	float	bx = 600 / ( xMax - xMin );
	float	ay = 40 - 400 * yMin / ( yMax - yMin );
	float	by = -400 / ( yMax - yMin );

	for ( i = 0; i < 64; i++ )
	{
		setrgbpalette ( i,      0, 0, i );
		setrgbpalette ( 64 + i, 0, i, 0 );
	}

	for ( i = 0; i < n1; i++ )
	{
		curPoint [i].x = x1 + i * hx;
		curPoint [i].y = y1;
		curPoint [i].z = f ( curPoint [i].x, curPoint [i].y );
		curLine [i].x = (int)( ax + bx * ( curPoint [i] & e1 ) );
		curLine [i].y = (int)( ay + by * ( curPoint [i] & e2 ) );
	}

	for ( i = 1; i < n2; i++ )
	{
		for ( j = 0; j < n1; j++ )
		{
			nextPoint [j].x = x1 + j * hx;
			nextPoint [j].y = y1 + i * hy;
			nextPoint [j].z = f ( nextPoint [j].x, nextPoint [j].y );
			nextLine [j].x = (int)( ax + bx * ( nextPoint [j] & e1 ) );
			nextLine [j].y = (int)( ay + by * ( nextPoint [j] & e2 ) );
		}

		for ( j = 0; j < n1 - 1; j++ )
		{
						// draw 1st triangle
			edge1 = curPoint [j+1] - curPoint [j];
			edge2 = nextPoint [j]  - curPoint [j];
			n     = edge1 ^ edge2;

			if ( ( n & e3 ) >= 0 )
				color = 64 + (int)( 20 + 43 * ( n & e3 ) / n.length () );
			else
				color = (int)( 20 - 43 * ( n & e3 ) / n.length () );

			setfillstyle ( SOLID_FILL, color );
			setcolor ( color );

			facet [0] = curLine [j];
			facet [1] = curLine [j+1];
			facet [2] = nextLine [j];

			fillpoly ( 3, ( int far * ) facet );

						// draw 2nd triangle
			edge1 = nextPoint [j+1] - curPoint [j+1];
			edge2 = nextPoint [j]   - curPoint [j+1];
			n     = edge1 ^ edge2;

			if ( ( n & e3 ) >= 0 )
			{
				color = 127;
				color = 64 + (int)( 20 + 43 * ( n & e3 ) / n.length () );
			}
			else
			{
				color = 63;
				color = (int)( 20 - 43 * ( n & e3 ) / n.length () );

			}

			setfillstyle ( SOLID_FILL, color );
			setcolor ( color );

			facet [0] = curLine  [j+1];
			facet [1] = nextLine [j];
			facet [2] = nextLine [j+1];

			fillpoly ( 3, ( int far * ) facet );
		}

		for ( j = 0; j < n1; j++ )
		{
			curLine  [j] = nextLine [j];
			curPoint [j] = nextPoint [j];
		}
	}

	delete curLine;
	delete nextLine;
	delete curPoint;
	delete nextPoint;
}

float	f ( float x, float y )
{
	float	r =  x*x + y*y;

//	return 0.5*sin(2*x)*sin(2*y);
//	return cos ( 2.2*r ) / ( r + 1 );
//	return sin ( r ) / ( r + 1 );
	return cos ( r ) / ( r + 1 );
}

float	f2 ( float x, float y )
{
	float	r =  x*x + y*y;
//	return 0;
//	return cos ( 2.2*r ) / ( r + 1 );
	return cos ( r ) / ( r + 1 );	// good also
}

main ()
{
	int	driver;
	int	mode;
	int	res;

	if ( ( driver = installuserdriver ( "VESA", NULL ) ) == grError )
	{
		printf ( "\nCannot load extended driver" );
		exit ( 1 );
	}

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	plotShadedSurface ( -2, -2, 2, 2, f2, -0.5, 1, 30, 30 );

	getch ();
	closegraph ();
}
