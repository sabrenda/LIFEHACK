#include	"polygon.h"

int	pointInConvexPolygon ( Polygon& p, Vector2D& v )
{
	if ( p.numVertices < 1 )
		return 0;

	if ( p.numVertices == 1 )
		return v == p.vertices [0];

	if ( p.numVertices == 2 )
	{
		int	c = v.classify ( p.vertices [0], p.vertices [1] );

		return c == BETWEEN || c == ORIGIN || c == DESTINATION;
	}

	for ( int i = 0; i < p.numVertices; i++ )
		if ( v.classify ( p.vertices [i], p.vertices [(i+1)%p.numVertices] ) == LEFT )
			return 0;

	return -1;
}
