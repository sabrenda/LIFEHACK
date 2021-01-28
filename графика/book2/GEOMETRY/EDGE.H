#ifndef	__EDGE__
#define	__EDGE__

#include	"vector2d.h"

class	Edge
{
public:
	Vector2D org;
	Vector2D dest;

	Edge ( const Vector2D& p1, const Vector2D& p2 )
	{
		org  = p1;
		dest = p2;
	}

	Edge ( const Edge& e )
	{
		org  = e.org;
		dest = e.dest;
	}

	Edge () {}

	Edge&    flip ();
	Edge&	 rot ();

	Vector2D point ( float t )
	{
		return org + t * (dest - org);
	}

	int	 intersect ( const Edge&, float& );
};

enum		// types of edge intersection
{
	COLLINEAR, PARALLEL, SKEW
};

#endif
