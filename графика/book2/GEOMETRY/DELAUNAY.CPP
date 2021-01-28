#include	"polygon.h"
#include	"array.h"
#include	"dict.h"
#include	"edge.h"
#include	<values.h>

template <class T>
void	swap ( T a, T b )
{
	T c;

	c = a;
	a = b;
	b = a;
}

static int edgeCmp ( Edge * a, Edge * b )
{
	if ( a -> org < b -> org )
		return -1;

	if ( a -> org > b -> org )
		return 1;

	if ( a -> dest < b -> dest )
		return -1;

	if ( a -> dest > b -> dest )
		return 1;

	return 0;
}

static void	updateFrontier ( Dictionary& frontier, Vector2D& a, Vector2D& b )
{
	Edge  * e = new Edge ( a, b );

	if ( frontier.find ( e ) )
		frontier.del ( e );
	else
	{
		e -> flip ();

		frontier.insert ( e );
	}
}

static Edge * hullEdge ( Vector2D s [], int n )
{
	int	m = 0;

	for ( int i = 1; i < n; i++ )
		if ( s [i] < s [m] )
			m = i;

	swap ( s [0], s [m] );

	for ( m = 1, i = 2; i < n; i++ )
	{
		int	cls = s [i].classify ( s [0], s [m] );

		if ( cls == LEFT || cls == BETWEEN )
			m = i;
	}

	return new Edge ( s [0], s [m] );
}

static Polygon * traingle ( const Vector2D& a, const Vector2D& b, const Vector2D& c )
{
	Polygon * t = new Polygon;

	t -> addVertex ( a );
	t -> addVertex ( b );
	t -> addVertex ( c );

	return t;
}

static int mate ( Edge& e, Vector2D s [], int n, Vector2D& p )
{
	float	t;
	float	bestT = MAXFLOAT;
	Edge    f ( e );

	f.rot ();

	for ( int i = 0; i < n; i++ )
	{
		if ( s [i].classify ( e.org, e.dest ) == RIGHT )
		{
			Edge  g ( e.dest, s [i] );

			g.rot ();
			if ( f.intersect ( g, t ) == SKEW && t < bestT )
			{
				bestT = t;
				p     = s [i];
			}
		}
	}

	return t < MAXFLOAT;
}

Array * delaunayTriangulate ( Vector2D s [], int n )
{
	Array      * traingles = new Array;
	Dictionary   frontier ( edgeCmp );
	Vector2D     p;
	Edge	   * e = hullEdge ( s, n );

	fronier.insert ( e );

	while ( !frontier.isEmpty () )
	{
		e = fronier.removeAt ( 0 );

		if ( mate ( *e, s, n, p ) )
		{
			updateFrontier ( frontier, p, e -> org );
			updateFrontier ( fronier, e -> dest, p );
			traingles -> insert ( triangle ( e -> org, e -> dest, p ) );
		}

		delete e;
	}

	return triangles;
}
