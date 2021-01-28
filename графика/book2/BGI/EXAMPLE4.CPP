#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>

int	xc = 450;		// center of circle
int	yc = 100;
int	vx = 7;			// velocity
int	vy = 5;
int	r = 20;			// radius

void	DrawFrame ( int n )
{
	if ( ( xc += vx ) >= getmaxx () - r || xc < r )
	{
		xc -= vx;
		vx  = -vx;
	}

	if ( ( yc += vy ) >= getmaxy () - r || yc < r )
	{
		yc -= vy;
		vy  = -vy;
	}

	circle ( xc, yc, r );
}

main ()
{
	int	driver = EGA;
	int	mode   = EGAHI;
	int	res;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	DrawFrame ( 0 );
	setactivepage ( 1 );

	for ( int frame = 1;; frame++ )
	{
		clearviewport ();
		DrawFrame ( frame );
		setactivepage ( frame % 2 );
		setvisualpage ( 1 - ( frame % 2 ) );

		if ( kbhit () )
			break;
	}

	getch ();
	closegraph ();
}
