#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;
	int	i;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	setpalette ( 0, 0  );
	setpalette ( 1, 32 );
	setpalette ( 2, 4  );
	setpalette ( 3, 36 );

	bar ( 0, 0, getmaxx (), getmaxy () );

	for ( i = 0; i < 4; i++ )
	{
		setfillstyle ( SOLID_FILL, i );
		bar ( 120 + i*100, 75, 219 + i*100, 274 );
	}

	getch ();
	closegraph ();
}
