//
// Abstract array class
// Author: Alex V. Boreskoff
//
#include <stdlib.h>
#include <string.h>

#include "array.h"

void  * Array :: atRemove ( int n )
{
        int newCount = count - 1;

        if ( n < count )
        {
                void  * item = root [n];

                memmove   ( &root [n], &root [n + 1], (newCount - n) * sizeof ( void * ) );
                setLength ( newCount );

                return item;
        }

        return NULL;
}

int     Array :: atDelete ( int n )
{
        freeItem ( atRemove ( n ) );

        return TRUE;
}

int     Array :: atInsert ( int n, void * item )
{
        if ( n <= count )
        {
                setLength ( count + 1 );

                if ( n < count )
                        memmove   ( &root [n + 1], &root [n], (count - n) * sizeof( void * ) );

                root [n] = item;

                return TRUE;
        }

        return FALSE;
}

int     Array :: find ( void * item ) const
{
        for ( register int i = 0; i < count; i++ )
                if ( root [i] == item )
                        return i;

        return -1;
}

int     Array :: insert ( void * item )
{
        if ( find ( item ) == -1 )
                return atInsert ( count, item );

        return FALSE;
}
