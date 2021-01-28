#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>

void	PlotGraphic ( float a, float b, float (*f)( float ) )
{
	float	xStep = pow ( 10, floor ( log ( ( b - a ) ) / log ( 10.0 ) ) );
	float	xMin  = xStep * floor ( a / xStep );
	float	xMax  = xStep * ceil  ( b / xStep );
	float * fVal  = new float [ 100 ];

	for ( int i = 0; i < 100; i++ )
		fVal [i] = f ( a + i * ( b - a ) / 100.0 );

	float	yMin = fVal [0],
		yMax = fVal [0];

	for ( i = 1; i < 100; i++ )
		if ( fVal [i] < yMin )
			yMin = fVal [i];
		else
		if ( fVal [i] > yMax )
			yMax = fVal [i];

	float	yStep = pow ( 10, floor ( log ( ( yMax - yMin ) ) / log ( 10.0 ) ) );

	yMin  = yStep * floor ( yMin / yStep );
	yMax  = yStep * ceil  ( yMax / yStep );

	int	x0 = 60;
	int	x1 = getmaxx () - 20;
	int	y0 = 10;
	int	y1 = getmaxy () - 40;

	line ( x0, y0, x1, y0 );
	line ( x1, y0, x1, y1 );
	line ( x1, y1, x0, y1 );
	line ( x0, y1, x0, y0 );

	float	kx = ( x1 - x0 ) / ( xMax - xMin );
	float	ky = ( y1 - y0 ) / ( yMax - yMin );
	float	x  = a;
	float	h  = ( b - a ) / 100.0;

	moveto ( x0 + ( x - xMin ) * kx, y0 + ( yMax - fVal [0] ) * ky );

	for ( i = 1; i < 100; i++, x += h )
		lineto ( x0 + ( x - xMin ) * kx, y0 + ( yMax - fVal [i] ) * ky );

	char	str [128];

	settextstyle ( SMALL_FONT, HORIZ_DIR, 1 );

	for ( x = xMin; x <= xMax; x += xStep )
	{
		int	ix = x0 + ( x - xMin ) * kx;

		line    ( ix, y1, ix, y1 + 10 );

		if ( x + xStep <= xMax )
			for ( i = 1; i < 10; i++ )
				line ( ix + i*xStep*kx*0.1, y1, ix + i*xStep*kx*0.1, y1 + 5 );

		sprintf ( str, "%g", x );
		outtextxy ( ix - textwidth ( str ) / 2, y1 + 15, str );
	}

	for ( float y = yMin; y <= yMax; y += yStep )
	{
		int	iy = y0 + ( yMax - y ) * ky;

		line    ( x0 - 10, iy, x0, iy );

		if ( y + yStep <= yMax )
			for ( i = 1; i < 10; i++ )
				line ( x0 - 5, iy - i*yStep*ky*0.1, x0, iy - i*yStep*ky*0.1 );

		sprintf ( str, "%g", y );
		outtextxy ( x0 - 10 - textwidth ( str ), iy, str );
	}

	delete fVal;
}

float	f ( float x )
{
	return sin ( x * x ) / ( x * x + 0.01 );
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

	PlotGraphic ( 0, 4, f );

	getch ();
	closegraph ();
}
