#include	"polygon.h"

Vector2D originPt;

int	polarCmp ( Vector2D * p, Vector2D * q )
{
	Vector2D vp     = *p - originPt;
	Vector2D vq     = *q - originPt;
	float	 pAngle = vp.polarAngle ();
	float	 qAngle = vq.polarAngle ();

	if ( pAngle < qAngle )
		return -1;

	if ( pAngle > qAngle )
		return 1;

	float	pLen = vp.length ();
	float	qLen = vq.length ();

	if ( pLen < qLen )
		return -1;

	if ( pLen > qLen )
		return 1;

	return 0;
}

Polygon * starPolgon ( Vector2D s [], int n )
{
	Polygon * p = new Polygon ( s, 1 );

	originPt = s [0];

	for ( int i = 1; i < n; i++ )
	{
		for ( int j = 1; polarCmp ( &s [i], &p->vertices [j] ) < 0; )
			if ( ++j >= p -> numVertices )
				j = 0;

		p -> addVertex ( s [i], j - 1 );
	}

	return p;
}
