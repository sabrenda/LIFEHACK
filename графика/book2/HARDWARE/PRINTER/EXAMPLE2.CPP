#include	<bios.h>
#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

int	port = 0;		// use LPT1:

inline	int	print ( char byte )
{
	return biosprint ( 0, byte, port );
}

int	printStr ( char * str )
{
	int	st;

	while ( *str != '\0' )
		if ( ( st = print ( *str++ ) ) & 1 )
			return st;

	return 0;
}

void	printScreenLJ ( int x1, int y1, int x2, int y2 )
{
	int	numCols   = x2 - x1 + 1;
	int	byte;
	char	str [20];

	printStr ( "\x1B*t150R" );		// set density 150 dpi
	printStr ( "\x1B&a5C" );		// move cursor to col 5
	printStr ( "\x1B*r1A" );		// begin raster graphics

	sprintf ( str, "\x1B*b%dW", (numCols+7)>>3);	// prepare line header

	for ( int y = y1; y <= y2; y++ )
	{
		printStr ( str );

		for ( int x = x1; x <= x2; )
		{
			byte = 0;
			for ( int i = 0; i < 8 && x <= x2; i++, x++ )
				if ( getpixel ( x, y ) > 0 )
					byte |= 0x80 >> i;

			Print ( byte );
		}
	}

	printStr ( "\x1B*rB" );
}

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res ) );
		exit ( 1 );
	}

	line ( 0, 0, 0, getmaxy () );
	line ( 0, getmaxy (), getmaxx (), getmaxy () );
	line ( getmaxx (), getmaxy (), getmaxx (), 0 );
	line ( getmaxx (), 0, 0, 0 );

	for ( int i = TRIPLEX_FONT; i <= GOTHIC_FONT; i++ )
	{
		settextstyle ( i, HORIZ_DIR, 5 );
		outtextxy ( 100, 50*i, "Some string" );
	}

	getch ();

	printScreenLJ ( 0, 0, getmaxx (), getmaxy () );

	closegraph ();
}
