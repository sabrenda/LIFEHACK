#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>

struct	Point
{
	int	x;
	int	y;
};

void	PlotIsolines ( float x0, float y0, float x1, float y1, float (*f)( float, float ), int n1, int n2, int nLines )
{
	float * fVal = new float [n1*n2];
	float	hx   = ( x1 - x0 ) / n1;
	float	hy   = ( y1 - y0 ) / n2;

	int	xStep = getmaxx () / n1;
	int	yStep = getmaxy () / n2;

	moveto ( 0, 0 );
	lineto ( ( n1 - 1 ) * xStep, 0 );
	lineto ( ( n1 - 1 ) * xStep, ( n2 - 1 ) * yStep );
	lineto ( 0, ( n2 - 1 ) * yStep );
	lineto ( 0, 0 );

	for ( int i = 0; i < n1; i++ )
		for ( int j = 0; j < n2; j++ )
			fVal [i + j*n1] = f ( x0 + i*hx, y0 + j*hy );

	float	zMin = fVal [0];
	float	zMax = fVal [0];

	for ( i = 0; i < n1 * n2; i++ )
		if ( fVal [i] < zMin )
			zMin = fVal [i];
		else
		if ( fVal [i] > zMax )
			zMax = fVal [i];

	float	dz = ( zMax - zMin ) / nLines;
	Point	p [3];

	for ( i = 0; i < n1 - 1; i++ )
		for ( int j = 0; j < n2 - 1; j++ )
		{
			int	k = i + j*n1;
			int	x = i * xStep;
			int	y = j * yStep;
			float	t;

			for ( float z = zMin; z <= zMax; z += dz )
			{
				int	count = 0;

				// edge 0-1
				if ( fVal [k] != fVal [k+1] )
				{
					t = ( z - fVal [k] ) / ( fVal [k+1] - fVal [k] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = (int)( x + t * xStep );
						p [count++].y = y;
					}
				}

				// edge 1-3
				if ( fVal [k+1] != fVal [k+n1] )
				{
					t = ( z - fVal [k+n1] ) / ( fVal [k+1] - fVal [k+n1] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = (int)( x + t * xStep );
						p [count++].y = (int)( y + ( 1 - t ) * yStep );
					}
				}

				// edge 3-0
				if ( fVal [k] != fVal [k+n1] )
				{
					t = ( z - fVal [k+n1] ) / ( fVal [k] - fVal [k+n1] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = x;
						p [count++].y = y + ( 1 - t ) * yStep;
					}
				}

				if ( count > 0 )
					line ( p [0].x, p [0].y, p [1].x, p [1].y );

				if ( count > 2 ) 	// line through vertex
					line ( p [1].x, p [1].y, p [2].x, p [2].y );

				count = 0;

				// edge 1-2
				if ( fVal [k+1] != fVal [k+n1+1] )
				{
					t = ( z - fVal [k+1] ) / ( fVal [k+n1+1] - fVal [k+1] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = x + xStep;
						p [count++].y = y + t * yStep;
					}
				}

				// edge 2-3
				if ( fVal [k+n1] != fVal [k+n1+1] )
				{
					t = ( z - fVal [k+n1] ) / ( fVal [k+n1+1] - fVal [k+n1] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = x + t * xStep;
						p [count++].y = y + yStep;
					}
				}

				// edge 3-1
				if ( fVal [k+1] != fVal [k+n1] )
				{
					t = ( z - fVal [k+n1] ) / ( fVal [k+1] - fVal [k+n1] );

					if ( t >= 0 && t <= 1 )
					{
						p [count  ].x = (int)( x + t * xStep );
						p [count++].y = (int)( y + ( 1 - t ) * yStep );
					}
				}

				if ( count > 0 )
					line ( p [0].x, p [0].y, p [1].x, p [1].y );

				if ( count > 2 ) 	// line through vertex
					line ( p [1].x, p [1].y, p [2].x, p [2].y );
			}
		}

	delete fVal;
}

float	f ( float x, float y )
{
	return x * x + y * y + 0.5 * x * y + x;
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

	PlotIsolines ( -1, -1, 1, 1, f, 20, 20, 20 );

	getch ();
	closegraph ();
}
