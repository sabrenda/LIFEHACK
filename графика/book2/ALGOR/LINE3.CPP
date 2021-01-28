#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

void	swap ( int& a, int& b )
{
	int	c;

	c = a;
	a = b;
	b = c;
}

int	outCode ( int x, int y, int X1, int Y1, int X2, int Y2 )
{
	int code = 0;

	if ( x < X1 )
		code |= 0x01;

	if ( y < Y1 )
		code |= 0x02;

	if ( x > X2 )
		code |= 0x04;

	if ( y > Y2 )
		code |= 0x08;

	return code;
}

void	clipLine ( int x1, int y1, int x2, int y2,
		   int X1, int Y1, int X2, int Y2 )
{
	int	code1   = outCode ( x1, y1, X1, Y1, X2, Y2 );
	int	code2   = outCode ( x2, y2, X1, Y1, X2, Y2 );
	int	inside  = ( code1 | code2 ) == 0;
	int	outside = ( code1 & code2 ) != 0;

	while ( !outside && !inside )
	{
		if ( code1 == 0 )
		{
			swap ( x1, x2 );
			swap ( y1, y2 );
			swap ( code1, code2 );
		}

		if ( code1 & 0x01 )		// clip left
		{
			y1 += (long)(y2-y1)*(X1-x1)/(x2-x1);
			x1  = X1;
		}
		else
		if ( code1 & 0x02 )		// clip above
		{
			x1 += (long)(x2-x1)*(Y1-y1)/(y2-y1);
			y1  = Y1;
		}
		else
		if ( code1 & 0x04 )		// clip right
		{
			y1 += (long)(y2-y1)*(X2-x1)/(x2-x1);
			x1  = X2;
		}
		else
		if ( code1 & 0x08 )		// clip below
		{
			x1 += (long)(x2-x1)*(Y2-y1)/(y2-y1);
			y1  = Y2;
		}

		code1   = outCode ( x1, y1, X1, Y1, X2, Y2 );
		code2   = outCode ( x2, y2, X1, Y1, X2, Y2 );
		inside  = ( code1 | code2 ) == 0;
		outside = ( code1 & code2 ) != 0;
	}

	line ( x1, y1, x2, y2 );
}

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	int	clipX1 = 100,
		clipY1 = 50,
		clipX2 = 540,
		clipY2 = 300;

	rectangle ( clipX1, clipY1, clipX2, clipY2 );

	clipLine ( 1,   100, 600, 200, clipX1, clipY1, clipX2, clipY2 );
	clipLine ( 1,   100, 600, 500, clipX1, clipY1, clipX2, clipY2 );
	clipLine ( 300, 600, 510, 3,   clipX1, clipY1, clipX2, clipY2 );
	clipLine ( 300, 0,   610, 200, clipX1, clipY1, clipX2, clipY2 );
	clipLine ( 300, 0,   300, 500, clipX1, clipY1, clipX2, clipY2 );

	getch ();
	closegraph ();
}
