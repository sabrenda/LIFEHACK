#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

#include	"vector3d.h"
#include	"matrix.h"

#define	N1	40
#define	N2	20

Vector3D prjDir ( 0, 0, 1 );
Vector3D vertex [N1*N2];
Matrix	 trans = RotateX ( M_PI / 4 );

struct	Point			// screen point
{
	int	x, y;
};

struct	Facet
{
	int	 index [3];
	Vector3D n;
	float	 depth;
	float	 coeff;
} * torus, * tmp;

int	facetComp ( const void * v1, const void * v2 )
{
	Facet * f1 = (Facet *) v1;
	Facet * f2 = (Facet *) v2;

	if ( f1 -> depth < f2 -> depth )
		return -1;
	else
	if ( f1 -> depth > f2 -> depth )
		return 1;
	else
		return 0;
}

void	initTorus ()
{
	float	r1 = 5;
	float	r2 = 1;

	// 1. Create vertices
	for ( int i = 0, k = 0; i < N1; i++ )
	{
		float	phi = i * 2 * M_PI / N1;

		for ( int j = 0; j < N2; j++, k++ )
		{
			float	psi = j * 2 * M_PI / N2;

			vertex [k].x = ( r1 + r2 * cos ( psi ) ) * cos ( phi );
			vertex [k].y = ( r1 + r2 * cos ( psi ) ) * sin ( phi );
			vertex [k].z = r2 * sin ( psi );

			vertex [k] = trans * vertex [k];
			vertex [k].z += 10;
		}
	}

	// 2. Create facets
	for ( i = k = 0; i < N1; i++ )
		for ( int j = 0; j < N2; j++, k += 2 )
		{
			torus [k].index [0] = i*N2 + j;
			torus [k].index [1] = ((i+1)%N1)*N2 + j;
			torus [k].index [2] = ((i+1)%N1)*N2 + (j + 1) % N2;

			torus [k+1].index [0] = i*N2 + j;
			torus [k+1].index [1] = ((i+1)%N1)*N2 + (j + 1)%N2;
			torus [k+1].index [2] = i*N2 + (j + 1)%N2;
		}
}

void	drawTorus ()
{
					// compute normals & distances
	for ( int i = 0, count = 0; i < N1*N2*2; i++ )
	{
		torus [i].n     = ( vertex [torus [i].index [1]] - vertex [torus [i].index [0] ] ) ^
				  ( vertex [torus [i].index [2]] - vertex [torus [i].index [1] ] );

		torus [i].coeff = ( torus [i].n & prjDir ) / !torus [i].n;
		torus [i].depth = vertex [torus [i].index [0]] & prjDir;

		for ( int j = 1; j < 3; j++ )
		{
			float	d = vertex [torus [i].index [j]] & prjDir;

			if ( d < torus [i].depth )
				torus [i].depth = d;
		}

		if ( torus [i].coeff > 0 )
			tmp [count++] = torus [i];
	}

					// sort them
	qsort ( tmp, count, sizeof ( Facet ), facetComp );

	Point	edges [3];
					// draw them
	for ( i = 0; i < count; i++ )
	{
		for ( int k = 0; k < 3; k++ )
		{
			edges [k].x = 320 + 30 * vertex [tmp [i].index [k]].x;
			edges [k].y = 240 + 30 * vertex [tmp [i].index [k]].y;
		}

		int	color = 64 + (int)( 20 + 43 * tmp [i].coeff );

		setfillstyle ( SOLID_FILL, color );
		setcolor     ( color );
		fillpoly     ( 3, (int far *) edges );
	}
}

main ()
{
	int	driver;
	int	mode = 2;
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

	for ( int i = 0; i < 64; i++ )
		setrgbpalette ( 64 + i, i, i, i );

	torus = new Facet [N1*N2*2];
	tmp   = new Facet [N1*N2*2];

	initTorus ();
	drawTorus ();

	getch ();
	closegraph ();

	delete tmp;
	delete torus;
}
