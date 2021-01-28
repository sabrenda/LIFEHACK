#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

void	Swap ( int& a, int& b )
{
	int	c;

	c = a;
	a = b;
	b = c;
}

int	OutCode ( int x, int y, int X1, int Y1, int X2, int Y2 )
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

void	ClipLine ( int x1, int y1, int x2, int y2,
		   int X1, int Y1, int X2, int Y2 )
{
	int	code1   = OutCode ( x1, y1, X1, Y1, X2, Y2 );
	int	code2   = OutCode ( x2, y2, X1, Y1, X2, Y2 );
	int	inside  = ( code1 | code2 ) == 0;
	int	outside = ( code1 & code2 ) != 0;

	while ( !outside && !inside )
	{
		if ( code1 == 0 )
		{
			Swap ( x1, x2 );
			Swap ( y1, y2 );
			Swap ( code1, code2 );
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

		code1   = OutCode ( x1, y1, X1, Y1, X2, Y2 );
		code2   = OutCode ( x2, y2, X1, Y1, X2, Y2 );
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

	int	ClipX1 = 100,
		ClipY1 = 50,
		ClipX2 = 540,
		ClipY2 = 300;

	rectangle ( ClipX1, ClipY1, ClipX2, ClipY2 );

	ClipLine ( 1,   100, 600, 200, ClipX1, ClipY1, ClipX2, ClipY2 );
	ClipLine ( 1,   100, 600, 500, ClipX1, ClipY1, ClipX2, ClipY2 );
	ClipLine ( 300, 600, 510, 3,   ClipX1, ClipY1, ClipX2, ClipY2 );
	ClipLine ( 300, 0,   610, 200, ClipX1, ClipY1, ClipX2, ClipY2 );
	ClipLine ( 300, 0,   300, 500, ClipX1, ClipY1, ClipX2, ClipY2 );
//	ClipLine ( 200, 100, 500, 200, ClipX1, ClipY1, ClipX2, ClipY2 );

	getch ();
	closegraph ();
}
