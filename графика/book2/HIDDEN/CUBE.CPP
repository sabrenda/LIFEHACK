#include	<Bios.h>
#include        <Graphics.h>
#include	"Vector3D.h"
#include	"Matrix.h"

struct	Point
{
	int	x, y;
};

struct	Edge
{
	int	v1, v2;		// vertices indexes
	int	f1, f2;		// facet's indexes
};

struct	Facet
{
	int	 v [4];		// vertices indexes
	Vector3D n;		// normal
	int      flags;
};

class	Cube
{
public:
	Vector3D vertex [8];
	Edge	 edge [12];
	Facet	 facet [6];

	Cube ();

	void	initEdge ( int i, int v1, int v2, int f1, int f2 )
	{
		edge [i].v1 = v1;
		edge [i].v2 = v2;
		edge [i].f1 = f1;
		edge [i].f2 = f2;
	};

	void	initFacet ( int i, int v1, int v2, int v3, int v4 )
	{
		facet [i].v [0] = v1;
		facet [i].v [1] = v2;
		facet [i].v [2] = v3;
		facet [i].v [3] = v4;
	};

	void	computeNormals ();
	int	isFrontFacing ( int i , Vector3D& v ) { return ((vertex [facet [i].v [0]] - v ) & facet [i].n) < 0; }
	void	apply ( Matrix& );
	void	draw ();
};

/////////////////////////////////////////////////////////////////

Matrix	 prj ( 1 );			// projection matrix
Vector3D eye ( 0, 0, 0 );		// observer loc
Vector3D light ( 0, 0.7, 0.7 );

////////////////////////////////////////////////////////////////

Cube :: Cube ()
{
	// 1. init vertices

	for ( int i = 0; i < 8; i++ )
	{
		vertex [i].x = i & 1 ? 1.0 : 0.0;
		vertex [i].y = i & 2 ? 1.0 : 0.0;
		vertex [i].z = i & 4 ? 1.0 : 0.0;
	}

	// 2. init edges

	initEdge ( 0,  0, 1, 2, 4 );
	initEdge ( 1,  1, 3, 1, 4 );
	initEdge ( 2,  3, 2, 3, 4 );
	initEdge ( 3,  2, 0, 0, 4 );
	initEdge ( 4,  4, 5, 2, 5 );
	initEdge ( 5,  5, 7, 1, 5 );
	initEdge ( 6,  7, 6, 3, 5 );
	initEdge ( 7,  6, 4, 0, 5 );
	initEdge ( 8,  0, 4, 0, 2 );
	initEdge ( 9,  1, 5, 1, 2 );
	initEdge ( 10, 3, 7, 1, 3 );
	initEdge ( 11, 2, 6, 0, 3 );

	// 3. init facets

	initFacet ( 0, 4, 6, 2, 0 );
	initFacet ( 1, 1, 3, 7, 5 );
	initFacet ( 2, 0, 1, 5, 4 );
	initFacet ( 3, 6, 7, 3, 2 );
	initFacet ( 4, 2, 3, 1, 0 );
	initFacet ( 5, 4, 5, 7, 6 );
}

void	Cube :: computeNormals ()
{
	for ( int i = 0; i < 6; i++ )
		facet [i].n = ( vertex [facet [i].v [1]] - vertex [facet [i].v [0]] ) ^
			      ( vertex [facet [i].v [2]] - vertex [facet [i].v [1]] );
}

void	Cube :: apply ( Matrix& m )
{
	for ( int i = 0; i < 8; i++ )
		vertex [i] = m * vertex [i];
}

void	Cube :: draw ()
{
	Point	 p [8];
	Point	 contour [4];
	Vector3D v;
	int	 color;

				// project vertices
	for ( int i = 0; i < 8; i++ )
	{
		v      = prj * vertex [i];
		p [i].x = (int) v.x;
		p [i].y = (int) v.y;
	}

	computeNormals ();

	for ( i = 0; i < 6; i++ )
		facet [i].flags = isFrontFacing ( i, eye );

				// draw all faces
	for ( i = 0; i < 6; i++ )
		if ( facet [i].flags )
		{
			int	color = -( facet [i].n & light ) * 7 + 8;

			for ( int j = 0; j < 4; j++ )
				contour [j] = p [facet [i].v [j]];

			setcolor ( color );
			setfillstyle ( SOLID_FILL, color );
			fillpoly ( 4, (int far *) contour );
		}
}

main ()
{
	Cube		cube;
	Matrix		trans;
	int		drv  = VGA;
	int		mode = VGAMED;
	palettetype     pal;
				// prepare projection matrix
	prj.x [2][2] = 0;
	prj.x [3][2] = 1;
	prj = scale ( Vector3D ( 640, 350, 0 ) ) * prj;
	prj = translate ( Vector3D ( 320, 175, 0 ) ) * prj;

	cube.apply ( translate ( Vector3D ( 1, 1, 4 ) ) );

	initgraph ( &drv, &mode, "C:\\BORLANDC\\BGI" );

	getpalette ( &pal );

	for ( int i = 0; i < pal.size; i++ )
		setrgbpalette ( pal.colors [i], (63*i)/15, (63*i)/15, (63*i)/15 );

	cube.draw ();

	bioskey ( 0 );
	closegraph ();
}
