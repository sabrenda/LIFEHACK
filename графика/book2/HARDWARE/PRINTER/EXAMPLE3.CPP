#include	<bios.h>
#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

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

void	printScreenPS ( int x1, int y1, int x2, int y2, int mode )
{
	int	xSize = x2 - x1 + 1;
	int	ySize = y2 - y1 + 1;
	int	numCols = ( x2 - x1 + 8 ) >> 3;
	int	byte, bit;
	char	str [20];

	printStr ( "/bmap_wid " );
	itoa ( xSize, str, 10 );
	printStr ( str );

	printStr ( "/bmap_hgt " );
	itoa ( ySize, str, 10 );
	printStr ( str );

	printStr ( "/bpp 1 def\n" );
	printStr ( "/res " );
	itoa ( mode, str, 10 );
	printStr ( str );
	printStr ( " def\n\n" );
	printStr ( "/x 5 def\n" );
	printStr ( "/y 5 def\n" );
	printStr ( "/scx 100 100 div def\n" );
	printStr ( "/scy 100 100 div def\n" );
	printStr ( "/scg 100 100 div def\n" );
	printStr ( "scaleit\n" );
	printStr ( "imagedata\n\n" );

	for ( int y = y1; y <= y2; y++ )
	{
		for ( int i = 0, x = x1; i < numCols; i++ )
		{
			for ( int j = 0, bit = 0x80, byte = 0; j < 8 && x + j <= x2; j++, bit >>= 1 )
				if ( getpixel ( x + j, y ) > 0 )
					byte |= bit;

			itoa ( byte, str, 16 );
			printStr ( str );
		}

		printStr ( "\n" );
	}

	printStr ( "\nshowit\n" );
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

	printScreenPS ( 0, 0, getmaxx (), getmaxy (), 100 );

	closegraph ();
}
