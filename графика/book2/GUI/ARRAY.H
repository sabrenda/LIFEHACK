//
// Abstract array class
// Author: Alex V. Boreskoff
//
#ifndef __ARRAY__
#define __ARRAY__

#include	"contnr.h"

class	Array : public Container
{
public:
	Array ( int aLimit = 8, int aThreshold = 16 ) : Container ( aLimit, aThreshold ) {}

	virtual	char * getClassName () const
	{
		return "Array";
	}

	void * & operator [] ( int n )	  // Get a reference to n-th element
	{
		if ( n >= count )
			setLength ( n + 1 );

		return root [n];
	}

	void * & at ( int n ) const
	{
		return root [n];
	}

	int 	find     ( void * item ) const;	// Find a element in array and return its index (or -1 if not found)
	int	atDelete ( int n );		// Delete element number 'n' from array
	int	atInsert ( int n, void * item);	// Insert element 'item' before element 'n'
	int	insert   ( void * );		// insert if not present
	void  * atRemove ( int n );		// remove item & return it
};

#endif
