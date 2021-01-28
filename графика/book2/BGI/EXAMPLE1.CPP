#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

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

	line ( 0, 0, 0, getmaxy () );
	line ( 0, getmaxy (), getmaxx (), getmaxy () );
	line ( getmaxx (), getmaxy (), getmaxx (), 0 );
	line ( getmaxx (), 0, 0, 0 );

	getch ();
	closegraph ();
}
