//
// Dictionary class
// Author: Alex V. Boreskoff
//
#include        "dict.h"

int     ptrCompare ( const void * p1, const void * p2 )
{
	if ( p1 < p2 )
		return -1;
	else
	if ( p1 > p2 )
		return 1;

        return 0;
}

int     Dictionary :: find ( const void * item, int& index ) const
{
	for ( int l = 0, h = count - 1; l <= h; )
	{
		int     m   = ( l + h ) >> 1;
		int     res = compFunc ( root [m], item );

                if ( res < 0 )  // root [m] < item
                        l = m + 1;
                else
                if ( res > 0 )  // root [m] > item
                        h = m - 1;
                else            // root [m] == item
                {
                        index = m;

                        return TRUE;
                }
        }

	index = l;
	return FALSE;
}

int     Dictionary :: insert ( void * item )
{
        int     index;

        if ( !find ( item, index ) )
                return atInsert ( index, item );

        return FALSE;
}

int     Dictionary :: del ( void * item )
{
        int     index;

        if ( !find ( item, index ) )
                return FALSE;

        return atDelete ( index );
}
