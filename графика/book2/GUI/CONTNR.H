//
// Basic class for all containers
// Author: Alex V. Boreskoff
//
#ifndef __CONTAINER__
#define __CONTAINER__

#include        <malloc.h>
#include        "object.h"

class   Container : public Object
{
protected:

        int     count;
        int     limit;
        int     threshold;
        void ** root;
public:
                        // Initialize object to hold initially 'aLimit' elements, 
                        // and increase storage by 'aThreshold' each time the 
                        // upper bound is exceeded.
        Container ( int aLimit = 8, int aThreshold = 16 );

        virtual ~Container ()
        {
                if ( root != NULL )
                        free ( root );
        }

        virtual int     isOk () const
        {
                return root != NULL;
        }

        virtual char * getClassName () const
        {
                return "Container";
        }

        virtual void freeItem ( void * item) {} // free item

        void    deleteAll ();                   // delete all items
        void    setLength ( int newLength );    // set size of container
        int     getCount () const               // return # of objects in container
        {
                return count;
        }

        int     isEmpty () const
        {
                return count < 1;
        }
};

#endif
