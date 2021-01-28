#include	<conio.h>
#include	<graphics.h>
#include	<mem.h>
#include	<process.h>
#include	<stdio.h>
#include	"sbuffer.h"

Vector3D p1 [] =
{
	Vector3D ( 60, 0, 1 ), Vector3D ( 480, 200, 3 ), Vector3D ( 0, 40, 1 )
};

Vector3D p2 [] =
{
	Vector3D ( 0, 200, 1 ), Vector3D ( 220, 0, 3 ), Vector3D ( 60, 260, 1 )
};

Vector3D p3 [] =
{
	Vector3D ( 20, 160, 3 ), Vector3D ( 420, 60, 1 ), Vector3D ( 460, 120, 1 )
};

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


	pool = new SpanPool ( 1500 );

	SBuffer	  sb ( 480 );
	Polygon3D poly1 ( p1, 3 );
	Polygon3D poly2 ( p2, 3 );
	Polygon3D poly3 ( p3, 3 );

	sb.addPoly ( poly1 );
	sb.addPoly ( poly2 );
	sb.addPoly ( poly3 );

	for ( int i = 0; i < sb.screenHeight; i++ )
	{
		Span * s = sb.head [i] -> next;

		while ( s != NULL )
		{
			int	color;

			if ( s -> facet == &poly1 )
				color = BLUE;
			else
			if ( s -> facet == &poly2 )
				color = GREEN;
			else
			if ( s -> facet == &poly3 )
				color = RED;
			else
				color = WHITE;

			setcolor ( color );
			line ( (int) s->x1, i, (int) s->x2, i );

			s = s-> next;
		}
	}

	getch ();

	closegraph ();
}
