#ifndef	__POINT__
#define __POINT__

struct Point
{
	int x, y;

	Point& operator += ( Point& );
	Point& operator -= ( Point& );
	Point  operator - ( Point& );
	Point  operator + ( Point& );
	friend int    operator == ( Point&, Point& );
	friend int    operator != ( Point&, Point& );
};

inline Point& Point :: operator += ( Point& p )
{
    x += p.x;
    y += p.y;
    return *this;
}

inline Point& Point :: operator -= ( Point& p )
{
    x -= p.x;
    y -= p.y;
    return *this;
}

inline	Point Point :: operator - ( Point& p )
{
	Point result;

	 result.x = x - p.x;
	 result.y = y - p.y;

	return result;
}

inline	Point Point :: operator + ( Point& p )
{
	Point result;

	 result.x = x + p.x;
	 result.y = y + p.y;

	return result;
}

inline	int operator == ( Point& p1, Point& p2 )
{
	return p1.x == p2.x && p1.y == p2.y;
}

inline	int operator != ( Point& p1, Point& p2 )
{
	return p1.x != p2.x || p1.y != p2.y;
}

#endif
