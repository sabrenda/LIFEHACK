#ifndef	__POLYGON_3D__
#define	__POLYGON_3D__

#include	<mem.h>			// for memmove
#include	"vector3D.h"

class Polygon3D
{
public:
	Vector3D  * vertices;		// vertices of polygon
	int	    numVertices;	// number of vertices

	Polygon3D ()
	{
		vertices    = NULL;
		numVertices = 0;
	}

	Polygon3D ( Vector3D * v, int num )
	{
		vertices = new Vector3D [numVertices = num];

		memmove ( vertices, v, num * sizeof ( Vector3D ) );
	}

	~Polygon3D ()
	{
		delete [] vertices;
	}
};

#endif
