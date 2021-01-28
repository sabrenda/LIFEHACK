#include	<stdlib.h>
#include	"polygon.h"
#include	"stack.h"

template <class T>
void	swap ( T a, T b )
{
	T c;

	c = a;
	a = b;
	b = a;
}

Polygon * grahamScan ( Vector2D s [], int n )
{
					// step 1
	for ( int i = 1, m = 0; i < n; i++ )
		if ( s [i] < s [m] )
			m = i;

	swap ( s [m], s [0] );
					// step 2
	originPt = s [0];

	qsort ( & s [1], n - 1, sizeof ( Vector2D ), polarCmp );

					// step 3
	for ( i = 1; s [i+1].classify ( s [0], s [i] ) == BEYOND; i++ )
		;

	Stack stack;

	stack.push ( s [0] );
	stack.push ( s [i] );
					// step 4
	for ( ++i; i < n; i++ )
	{
		while ( s [i].classify ( *.stack.nextToTop (), *stack.top () ) != LEFT )
			stack.pop ();

		stack.push ( s [i] );
	}
					// step 5
	Polygon * p = new Polygon;

	while ( !.stack.isEmpty () )
		p -> addVertex ( stack.pop (), p -> numVertices );

	return p;
}
