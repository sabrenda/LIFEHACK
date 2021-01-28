#include	"polygon.h"

int	Polygon :: addVertex ( const Vector2D& v, int after )
{
	if ( numVertices + 1 >= maxVertices )
		resize ( maxVertices + 5 );

	if ( after >= numVertices )
		vertices [numVertices] = v;
	else
	{
		memmove ( vertices + after, vertices + after + 1,
			  (numVertices - after) * sizeof ( Vector2D ) );

		vertices [after] = v;
	}

	return ++numVertices;
}

int	Polygon :: delVertex ( int index )
{
	if ( index < 0 || index >= numVertices )
		return 0;

	memmove ( &vertices [index], &vertices [index+1],
		  (numVertices - index - 1) * sizeof ( Vector2D ) );

	return --numVertices;
}

int	Polygon :: isInside ( const Vector2D& p )
{
	int	count = 0;		// count of ray/edge intersections

	for ( int i = 0; i < numVertices; i++ )
	{
		int	j = ( i + 1 ) % numVertices;

		if ( vertices [i].y == vertices [j].y )
			continue;

		if ( vertices [i].y > p.y && vertices [j].y > p.y )
			continue;

		if ( vertices [i].y < p.y && vertices [j].y < p.y )
			continue;

		if ( max ( vertices [i].y, vertices [j].y ) == p.y )
			count ++;
		else
		if ( min ( vertices [i].y, vertices [j].y ) == p.y )
			continue;
		else
		{
			float	t = (p.y - vertices [i].y)/(vertices [j].y - vertices [i].y);

			if ( vertices [i].x + t * (vertices [j].x - vertices [i].x) >= p.x )
				count ++;
		}
	}

	return count & 1;
}

Polygon * Polygon :: split ( int from, int to )
{
	Polygon * p = new Polygon;

	if ( to < from )
		to += numVertices;

	for ( int i = from; i <= to; i++ )
		p -> addVertex ( vertices [i % numVertices] );

	if ( to < numVertices )
		memmove ( vertices+from+1, vertices+to+1, (numVertices-to-1) * sizeof ( Vector2D ) );
	else
		memmove ( vertices, vertices+to, (numVertices-to) * sizeof ( Vector2D ) );

	numVertices -= to - from - 1;

	return p;
}

void	Polygon :: resize ( int newMaxVertices )
{
	if ( newMaxVertices < maxVertices )
		return;

	Vector2D * newVertices = new Vector2D [maxVertices = newMaxVertices];

	if ( vertices != NULL )
	{
		memcpy ( newVertices, vertices, numVertices * sizeof ( Vector2D ) );

		delete [] vertices;
	}
}
