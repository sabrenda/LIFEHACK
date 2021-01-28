#ifndef	__POINT__
#define __POINT__

class Point
{
public:
	int	x, y;

	Point () {}
	Point ( const Point& p )
	{
		x = p.x;
		y = p.y;
	}
	Point ( int ax, int ay )
	{
		x = ax;
		y = ay;
	}

	Point& operator += ( const Point& );
	Point& operator -= ( const Point& );
	Point  operator -  ( const Point& );
	Point  operator +  ( const Point& );
	friend int    operator == ( const Point&, const Point& );
	friend int    operator != ( const Point&, const Point& );
};

inline Point& Point :: operator += ( const Point& p )
{
    x += p.x;
    y += p.y;

    return *this;
}

inline Point& Point :: operator -= ( const Point& p )
{
    x -= p.x;
    y -= p.y;

    return *this;
}

inline	Point Point :: operator - ( const Point& p )
{
	Point result;

	result.x = x - p.x;
	result.y = y - p.y;

	return result;
}

inline	Point Point :: operator + ( const Point& p )
{
	Point result;

	result.x = x + p.x;
	result.y = y + p.y;

	return result;
}

inline	int operator == ( const Point& p1, const Point& p2 )
{
	return p1.x == p2.x && p1.y == p2.y;
}

inline	int operator != ( const Point& p1, const Point& p2 )
{
	return p1.x != p2.x || p1.y != p2.y;
}

#endif
