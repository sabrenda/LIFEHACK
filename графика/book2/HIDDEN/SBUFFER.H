//
// SBuffer management routines
//
// Author: Alex V. Boreskoff
//

#ifndef	__S_BUFFER__
#define	__S_BUFFER__

#include	<alloc.h>	// for NULL
#include	"polygon.h"

struct	Span			// span of S-Buffer
{
	float	x1, x2;		// endpoints of the span, as floats to speed up calculations
	float	invZ1, invZ2;	// 1/z values for endpoints
	float	k, b;		// coefficients of equation: 1/z=k*x+b

	Span  * next;		// next span
	Span  * prev;		// previous span;

	Polygon3D * facet;	// facet it belongs to

	float	f ( float x, float invZ ) const
	{
		return k * x + b - invZ;
	}
				// clip the span at the begining
	void	clipAtX1  ( float newX1 )
	{
		invZ1 = k * newX1 + b;
		x1    = newX1;
	}
				// clip the span at the end
	void	clipAtX2 ( float newX2 )
	{
		invZ2 = k * newX2 + b;
		x2    = newX2;
	}
				// precompute k and b coefficients
	void	precompute ()
	{
		if ( x1 != x2 )
		{
			k = (invZ2 - invZ1) / (x2 - x1);
			b = invZ1 - k * x1;
		}
		else
		{
			k = 0;
			b = invZ1;
		}
	}
				// whether the span is empty
	int	isEmpty ()
	{
		return x1 > x2;
	}
				// insert current span before s
	void	insertBefore ( Span * s )
	{
		prev              = s -> prev;
		s -> prev -> next = this;
		s -> prev         = this;
		next              = s;
	}
				// insert current span after s
	void	insertAfter ( Span * s )
	{
		if ( s -> next != NULL )
			s -> next -> prev = this;

		prev      = s;
		next      = s -> next;
		s -> next = this;
	}
				// remove current span from list
	void	remove ()
	{
		prev -> next = next;
		if ( next != NULL )
			next -> prev = prev;
	}
};

class	SpanPool
{
public:
	Span  * pool;		// pool of Span strctures
	int	poolSize;	// # of structures allocated
	Span  * firstAvailable;	// pointer to 1st struct after all allocated
	Span  * lastAvailable;	// last item in the pool
	Span  * free;		// pointer to free structs list (below firstAvailable)

	SpanPool ( int maxSize )
	{
		pool           = new Span [poolSize = maxSize];
		firstAvailable = pool;
		lastAvailable  = pool + poolSize - 1;
		free           = NULL;
	}

	~SpanPool ()
	{
		delete [] pool;
	}

	void	freeAll ()		// free all spans
	{
		firstAvailable = pool;
		free           = NULL;
	}

	Span  * allocSpan ( Span * s = NULL )	// allocate unused span
	{					// when no free spans
		if ( free == NULL )
			if ( firstAvailable <= lastAvailable )
			{
				firstAvailable -> next = NULL;
				firstAvailable -> prev = NULL;

				if ( s != NULL )
					* firstAvailable = * s;

				return firstAvailable++;
			}
			else
				return NULL;

		Span  * res = free;

		free = free -> next;

		res -> next = NULL;
		res -> prev = NULL;

		if ( s != NULL )
			* res = * s;

		return res;
	}
				// deallocate span (return to free span list)
	void	freeSpan ( Span * span )
	{
		span -> next = free;
		free         = span;
	}
};

extern	SpanPool * pool;

class	SBuffer
{
public:
	Span ** head;
	int	screenHeight;

	SBuffer ( int height )
	{
		head = new Span * [screenHeight = height];

		reset ();
	}

	~SBuffer ()
	{
		delete [] head;
	}

	void	reset ()
	{
		pool -> freeAll ();

		for ( register int i = 0; i < screenHeight; i++ )
		{
			head [i]         = pool -> allocSpan ();
			head [i] -> prev = NULL;
			head [i] -> next = NULL;
		}
	}

	void	addSpan ( int line, Span * span );
	void	addPoly ( const Polygon3D& poly );
};

int	compareSpans ( const Span * s1, const Span * s2 );
#endif