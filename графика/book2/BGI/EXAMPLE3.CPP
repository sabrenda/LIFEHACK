#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

main ()
{
	int		driver = VGA;
	int		mode   = VGAHI;
	int		res;
	palettetype     pal;

	initgraph ( &driver, &mode, "" );

	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	getpalette ( &pal );
	for ( int i = 0; i < pal.size; i++ )
	{
		setrgbpalette ( pal.colors [i], (63*i)/15, (63*i)/15, (63*i)/15 );
		setfillstyle  ( SOLID_FILL, i );
		bar ( i*40, 100, 39 + i*40, 379 );
	}

	getch ();
	closegraph ();
}
