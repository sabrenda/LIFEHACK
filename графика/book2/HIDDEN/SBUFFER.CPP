//
// SBuffer management routines
//
// Author: Alex V. Boreskoff
//
#include	<mem.h>
#include	"SBuffer.h"

SpanPool      * pool = NULL;
//
// compare spans s1 and s2, s1 being the inserted span
// return positive if s2 cannot obscure s1 (s1 is nearer)
//
int	compareSpans ( const Span * s1, const Span * s2 )
{
			// try to check max/min depths
	float	s1MinInvZ, s1MaxInvZ;
	float	s2MinInvZ, s2MaxInvZ;

			// compute min/max for 1/z for s1
	if ( s1 -> invZ1 < s1 -> invZ2 )
	{
		s1MinInvZ = s1 -> invZ1;
		s1MaxInvZ = s1 -> invZ2;
	}
	else
	{
		s1MinInvZ = s1 -> invZ2;
		s1MaxInvZ = s1 -> invZ1;
	}

			// compute min/max for 1/z for s2
	if ( s2 -> invZ1 < s2 -> invZ2 )
	{
		s2MinInvZ = s2 -> invZ1;
		s2MaxInvZ = s2 -> invZ2;
	}
	else
	{
		s2MinInvZ = s2 -> invZ2;
		s2MaxInvZ = s2 -> invZ1;
	}
			// compare inverse depths
	if ( s1MinInvZ >= s2MaxInvZ )
		return 1;

	if ( s2MinInvZ >= s1MaxInvZ )
		return -1;

			// if everything fails then try direct approach
	float	f1  = s1 -> f ( s2 -> x1, s2 -> invZ1 );
	float	f2  = s1 -> f ( s2 -> x2, s2 -> invZ2 );
	int	res = 0;

	if ( f1 <= 0 && f2 <= 0 )
		res = -1;
	else
	if ( f1 >= 0 && f2 >= 0 )
		res = 1;
	else
	{
		f1 = s2 -> f ( s1 -> x1, s1 -> invZ1 );
		f2 = s2 -> f ( s1 -> x2, s1 -> invZ2 );

		if ( f1 <= 0 && f2 <= 0 )
			res = 1;
		else
			res = -1;
	}

	return res;
}

////////////////////////// SBuffer methods ///////////////////////////////

void	SBuffer :: addSpan ( int line, Span * newSpan )
{
    newSpan -> precompute ();

    Span * prevSpan = head [line];
    Span * curSpan  = prevSpan -> next;

    for ( ; curSpan != NULL; )
    {
	if ( curSpan -> x2 < newSpan -> x1 )
	{
	    prevSpan = curSpan;
	    curSpan  = curSpan -> next;

	    continue;
	}

	if ( newSpan -> x1 < curSpan -> x1 )	// cases 1, 2 or 4
	{
	    if ( newSpan -> x2 <= curSpan -> x1 )	// case 1
	    {
		newSpan -> insertBefore ( curSpan );

		return;
	    }

	    if ( newSpan -> x2 < curSpan -> x2 )	// case 2
	    {
		if ( compareSpans ( curSpan, newSpan ) < 0 )
		    curSpan -> clipAtX1 ( newSpan -> x2 );
		else
		    newSpan -> clipAtX2 ( curSpan -> x1 );

		if ( !newSpan -> isEmpty () )
		    newSpan -> insertBefore ( curSpan );

		return;
	    }
	    else					// case 4
	    {
		if ( compareSpans ( curSpan, newSpan ) < 0 )
		{
		    Span * tempSpan = curSpan -> next;

		    curSpan -> remove ();		// remove from chain
		    pool    -> freeSpan ( curSpan );
		    curSpan  = tempSpan;

		    continue;
		}
		else
		{
		    Span * tempSpan = pool -> allocSpan ( newSpan );

		    tempSpan -> insertBefore ( curSpan );
		    tempSpan -> clipAtX2 ( curSpan -> x1 );
		    newSpan  -> clipAtX1 ( curSpan -> x2 );
		}
	    }
	}
	else		// curSpan -> x1 <= newSpan -> x1
	{
	    if ( newSpan -> x2 > curSpan -> x2 ) 	// case 5
	    {
		if ( compareSpans ( curSpan, newSpan ) < 0 )
		{
		    curSpan -> clipAtX2 ( newSpan -> x1 );

		    if ( curSpan -> isEmpty () )
		    {
			Span * tempSpan = curSpan -> next;

			curSpan -> remove ();
			pool    -> freeSpan ( curSpan );
			curSpan = tempSpan;

			continue;
		    }
		}
		else
		    newSpan -> clipAtX1 ( curSpan -> x2 );
	    }
	    else					// case 3
	    {
		if ( compareSpans ( curSpan, newSpan ) < 0 )
		{
		    Span * tempSpan = pool -> allocSpan ( curSpan );

		    curSpan  -> clipAtX2    ( newSpan -> x1 );
		    newSpan  -> insertAfter ( curSpan );
		    tempSpan -> clipAtX1    ( newSpan -> x2 );

		    if ( !tempSpan -> isEmpty () )
			tempSpan -> insertAfter ( newSpan );
		    else
			pool -> freeSpan ( tempSpan );

		    return;
		}
		else
		    return;
	    }
	}

	if ( newSpan -> isEmpty () )
		return;

	prevSpan = curSpan;
	curSpan  = curSpan -> next;
    }

    newSpan -> insertAfter ( prevSpan );
}

static	int	findEdge ( int& i, int dir, const Polygon3D& p )
{
	for ( ; ; )
	{
		int	i1 = i + dir;

		if ( i1 < 0 )
			i1 = p.numVertices - 1;
		else
		if ( i1 >= p.numVertices )
			i1 = 0;

		if ( p.vertices [i1].y < p.vertices [i].y )
			return -1;
		else
		if ( p.vertices [i1].y == p.vertices [i].y )
			i = i1;
		else
			return i1;
	}
}

void	SBuffer :: addPoly ( const Polygon3D& poly )
{
	int	yMin          = (int) poly.vertices [0].y;
	int	yMax          = (int) poly.vertices [0].y;
	int	topPointIndex = 0;

	for ( int i = 1; i < poly.numVertices; i++ )
		if ( poly.vertices [i].y < poly.vertices [topPointIndex].y )
			topPointIndex = i;
		else
		if ( poly.vertices [i].y > yMax )
			yMax = (int) poly.vertices [i].y;

	yMin = (int) poly.vertices [topPointIndex].y;

	if ( yMin == yMax )		// degenerate polygon
	{
		int	iMin = 0;
		int	iMax = 0;

		for ( i = 1; i < poly.numVertices; i++ )
			if ( poly.vertices [i].x < poly.vertices [iMin].x )
				iMin = i;
			else
			if ( poly.vertices [i].x > poly.vertices [iMax].x )
				iMax = i;

		Span * span = pool -> allocSpan ();

		span -> x1    = poly.vertices [iMin].x;
		span -> x2    = poly.vertices [iMax].x;
		span -> invZ1 = 1.0 / poly.vertices [iMin].z;
		span -> invZ2 = 1.0 / poly.vertices [iMax].z;
		span -> facet = (Polygon3D *) &poly;

		addSpan ( yMin, span );

		return;
	}

	int	i1, i1Next;
	int	i2, i2Next;

	i1     = topPointIndex;
	i1Next = findEdge ( i1, -1, poly );
	i2     = topPointIndex;
	i2Next = findEdge ( i2, 1, poly );

	float	x1     = poly.vertices [i1].x;
	float	x2     = poly.vertices [i2].x;
	float	invZ1  = 1.0 / poly.vertices [i1].z;
	float	invZ2  = 1.0 / poly.vertices [i2].z;
	float	dx1    = (poly.vertices [i1Next].x - poly.vertices [i1].x) / (poly.vertices [i1Next].y - poly.vertices [i1].y);
	float	dx2    = (poly.vertices [i2Next].x - poly.vertices [i2].x) / (poly.vertices [i2Next].y - poly.vertices [i2].y);
	float	dInvZ1 = (1.0/poly.vertices [i1Next].z - 1.0/poly.vertices [i1].z) / (poly.vertices [i1Next].y - poly.vertices [i1].y);
	float	dInvZ2 = (1.0/poly.vertices [i2Next].z - 1.0/poly.vertices [i2].z) / (poly.vertices [i2Next].y - poly.vertices [i2].y);
	int	y1Next = (int) poly.vertices [i1Next].y;
	int	y2Next = (int) poly.vertices [i2Next].y;

	for ( int y = yMin; y <= yMax; y++ )
	{
		Span * span = pool -> allocSpan ();

		if ( span == NULL )
			return;

		span -> x1    = x1;
		span -> invZ1 = invZ1;
		span -> x2    = x2;
		span -> invZ2 = invZ2;
		span -> facet = (Polygon3D *) &poly;

		addSpan ( y, span );

		x1    += dx1;
		x2    += dx2;
		invZ1 += dInvZ1;
		invZ2 += dInvZ2;

		if ( y + 1 == y1Next )
		{
			i1 = i1Next;
			if ( --i1Next < 0 )
				i1Next = poly.numVertices - 1;

			y1Next = (int) poly.vertices [i1Next].y;

			if ( poly.vertices [i1].y >= poly.vertices [i1Next].y )
				break;

			dx1    = (poly.vertices [i1Next].x - poly.vertices [i1].x) / (poly.vertices [i1Next].y - poly.vertices [i1].y);
			dInvZ1 = (1.0/poly.vertices [i1Next].z - 1.0/poly.vertices [i1].z) / (poly.vertices [i1Next].y - poly.vertices [i1].y);
		}

		if ( y + 1 == y2Next )
		{
			i2 = i2Next;
			if ( ++i2Next >= poly.numVertices )
				i2Next = 0;

			y2Next = (int) poly.vertices [i2Next].y;

			if ( poly.vertices [i2].y >= poly.vertices [i2Next].y )
				break;

			dx2    = (poly.vertices [i2Next].x - poly.vertices [i2].x) / (poly.vertices [i2Next].y - poly.vertices [i2].y);
			dInvZ2 = (1.0/poly.vertices [i2Next].z - 1.0/poly.vertices [i2].z) / (poly.vertices [i2Next].y - poly.vertices [i2].y);
		}
	}
}
