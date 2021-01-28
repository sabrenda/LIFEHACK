//
// Rect class
// Author: Alex V. Boreskoff
//
#ifndef	__RECT__
#define	__RECT__

#include	"point.h"

class Rect
{
public:
	int	x1, y1, x2, y2;

	Rect () {}
	Rect ( int ax, int ay, int bx, int by )
	{
		x1 = ax; y1 = ay; x2 = bx; y2 = by;
	}

	Rect&	operator &= ( const Rect& );
	Rect&	operator |= ( const Rect& );
	int 	operator == ( const Rect& r ) const;
	int 	operator != ( const Rect& r ) const;

	void	move     ( int dx, int dy );
	void	grow     ( int dx, int dy );
	int	contains ( const Point& p ) const;
	int	contains ( int x, int y ) const;
	int	isEmpty  () const;

	int	width () const
	{
		return x2 - x1 + 1;
	}

	int	height () const
	{
		return y2 - y1 + 1;
	}
};

inline void Rect :: move ( int dx, int dy )
{
	x1 += dx;
	y1 += dy;
	x2 += dx;
	y2 += dy;
}

inline void Rect :: grow ( int dx, int dy )
{
	x1 -= dx;
	y1 -= dy;
	x2 += dx;
	y2 += dy;
}

inline int Rect :: contains ( const Point& p ) const
{
	return ( p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2 );
}

inline int Rect :: contains ( int x, int y ) const
{
	return ( x >= x1 && x <= x2 && y >= y1 && y <= y2 );
}

inline int Rect :: operator == ( const Rect& r ) const
{
	return ( x1 == r.x1 && y1 == r.y1 && x2 == r.x2 && y2 == r.y2 );
}

inline int Rect :: operator != ( const Rect& r ) const
{
	return ! ( *this == r );
}

inline int Rect :: isEmpty () const
{
	return x1 > x2 || y1 > y2;
}

inline	Rect& Rect :: operator &= ( const Rect& r )
{
	if ( r.x1 > x1 )
		x1 = r.x1;

	if ( r.y1 > y1 )
		y1 = r.y1;

	if ( r.x2 < x2 )
		x2 = r.x2;

	if ( r.y2 < y2 )
		y2 = r.y2;

	return *this;
}

inline	Rect& Rect :: operator |= ( const Rect& r )
{
	if ( r.x1 < x1 )
		x1 = r.x1;

	if ( r.y1 < y1 )
		y1 = r.y1;

	if ( r.x2 > x2 )
		x2 = r.x2;

	if ( r.y2 > y2 )
		y2 = r.y2;

	return *this;
}

#endif
