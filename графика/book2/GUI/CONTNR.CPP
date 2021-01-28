//
// Basic class for all containers
// Author: Alex V. Boreskoff
//
#include <stdlib.h>
#include <string.h>

#include "contnr.h"

Container :: Container ( int aLimit, int aThreshold )
{
        root      = (void **) malloc ( (limit = aLimit) * sizeof ( void * ) );
        count     = 0;
        threshold = aThreshold;
}

void    Container :: deleteAll ()
{
        for ( int i = 0; i < count; i++ )
                freeItem ( root [i] );

        setLength ( 0 );
}

void    Container :: setLength ( int newLength )
{
        count = newLength;

        if ( ( newLength > limit ) || ( ( limit > threshold ) && ( newLength < limit - threshold ) ) )
        {
                newLength  = ((newLength + threshold - 1) / threshold) * threshold;
                root       = (void **) realloc ( root, newLength * sizeof (void *) );
                limit      = newLength;
        }
}

