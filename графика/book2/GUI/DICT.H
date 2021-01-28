//
// Dictionary class
// Author: Alex V. Boreskoff
//
#ifndef __DICTIONARY__
#define __DICTIONARY__

#include        "array.h"

int	ptrCompare ( const void *, const void * );

class   Dictionary : public Array
{
	int     (*compFunc)(const void *, const void *);
public:
	Dictionary ( int aLimit, int aThreshold,
		     int (*compare)(const void *, const void *) = ptrCompare ) : Array ( aLimit, aThreshold )
        {
                compFunc = compare;
        }

        virtual char  * getClassName () const
        {
                return "Dictionary";
        }

        int     insert ( void * );
        int     del    ( void * );
        int     find   ( const void *, int& ) const;
};

#endif
