#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

int	huge	MyDetect ( void )
{
	return 2;
}

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	installuserdriver ( "VESA", MyDetect );
	initgraph ( &driver, &mode, "" );

	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	for ( int i = 0; i < 64; i++ )
	{
		setrgbpalette ( i, i, i, 0 );
		setfillstyle ( SOLID_FILL, i );
		bar ( i*10, 0, 9 + i*10, getmaxy () );
	}

	getch ();
	closegraph ();
}
