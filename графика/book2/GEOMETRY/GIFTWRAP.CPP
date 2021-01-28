#include	"polygon.h"

template <class T>
void	swap ( T a, T b )
{
	T c;

	c = a;
	a = b;
	b = a;
}

Polygon * giftWrapHull ( Vector2D s [], int n )
{
	int	a = 0;
			// find leftmost point
	for ( int i = 1; i < n; i++ )
		if ( s [i].x < s [a].x )
			a = i;

	s [n] = s [a];

	Polygon * p = new Polygon;

	for ( i = 0; i < n; i++ )
	{
		swap ( s [a], s [i] );

		p -> addVertex ( s [i], p -> numVertices );

		a = i + 1;

		for ( int j = i + 2;  j <= n; j++ )
		{
			int	c = s [j].classify ( s [i], s [a] );

			if ( c == LEFT || c == BEYOND )
				a = i;
		}

		if ( a == n )
			return p;
	}

	return NULL;
}
