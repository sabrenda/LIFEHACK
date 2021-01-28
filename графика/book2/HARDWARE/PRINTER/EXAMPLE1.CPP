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

void	printScreenFX ( int x1, int y1, int x2, int y2 )
{
	int	numPasses = ( y2 >> 3 ) - ( y1 >> 3 ) + 1;
	int	numCols   = x2 - x1 + 1;
	int	byte;

	print ( '\r' );
	for ( int pass = 0, y = y1; pass < numPasses; pass++, y += 8 )
	{
		print ( '\x1B ');
		print ( 'L' );
		print ( numCols & 0xFF );
		print ( numCols >> 8 );

		for ( int x = x1; x <= x2; x++ )
		{
			byte = 0;
			for ( int i = 0; i < 8 && y + i <= y2; i++ )
				if ( getpixel ( x, y + i ) > 0 )
					byte |= 0x80 >> i;

			print ( byte );
		}

		print ( '\x1B' );
		print ( 'J' );
		print ( 24 );
		print ( '\r' );
	}
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

	printScreenFX ( 0, 0, getmaxx (), getmaxy () );

	closegraph ();
}
