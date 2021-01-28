#include	"edge.h"

Edge&	Edge :: rot ()
{
	Vector2D m = 0.5 * (org + dest);	// center of the edge
	Vector2D n ( 0.5*(dest.y  -org.y), 0.5*(org.x - dest.x) );

	org  = m - n;
	dest = m + n;

	return *this;
}

Edge&	Edge :: flip ()
{
	Vector2D tmp = org;

	org  = dest;
	dest = tmp;

	return *this;
}

int	Edge :: intersect ( const Edge& e, float& t )
{
	Vector2D n ( e.dest.y - e.org.y, e.org.x - e.dest.x );
	float	 denom = n & (dest - org);

	if ( denom == 0.0 )
	{
		int	cls = org.classify ( e.org, e.dest );

		if ( cls == LEFT || cls == RIGHT )
			return PARALLEL;

		return COLLINEAR;
	}

	t = - (n & (org - e.org)) / denom;

	return SKEW;
}
