#ifndef	__POLYGON__
#define	__POLYGON__

#include	<mem.h>
#include	"vector2d.h"

class	Polygon
{
public:
	int	   numVertices;		// current # of vertices
	int	   maxVertices;		// size of vertices array
	Vector2D * vertices;

	Polygon ()
	{
		numVertices = maxVertices = 0;
		vertices    = NULL;
	}

	Polygon ( const Vector2D * v, int size )
	{
		vertices = new Vector2D [numVertices = maxVertices = size];

		memcpy ( vertices, v, size * sizeof ( Vector2D ) );
	}

	Polygon ( const Polygon& p )
	{
		vertices    = new Vector2D [p.maxVertices];
		numVertices = p.numVertices;
		maxVertices = p.maxVertices;

		memcpy ( vertices, p.vertices, numVertices * sizeof ( Vector2D ) );
	}

	~Polygon ()
	{
		if ( vertices != NULL )
			delete [] vertices;
	}

	int	  addVertex ( const Vector2D& v )
	{
		return addVertex ( v, numVertices );
	}

	int	  addVertex ( const Vector2D& v, int after );
	int	  delVertex ( int index );
	int	  isInside  ( const Vector2D& );
	Polygon * split ( int from, int to );

protected:
	void	resize ( int newMaxVertices );
};

#endif
