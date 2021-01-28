#include "vector2d.h"

////////////////////// member functions //////////////////

int	Vector2D :: classify ( const Vector2D& p, const Vector2D& q ) const
{
	Vector2D a = q - p;
	Vector2D b = *this - p;
	float	 s = a.x * b.y - a.y * b.x;

	if ( s > 0.0 )
		return LEFT;

	if ( s < 0.0 )
		return RIGHT;

	if ( a.x * b.x < 0.0 || a.y * b.y < 0.0 )
		return BEHIND;

	if ( a.length () < b.length () )
		return BEYOND;

	if ( p == *this )
		return ORIGIN;

	if ( q == *this )
		return DESTINATION;

	return BETWEEN;
}

