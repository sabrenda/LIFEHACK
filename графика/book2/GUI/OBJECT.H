//
// Root class for all objects
// Author: Alex V. Boreskoff
//
#ifndef __OBJECT__
#define __OBJECT__

#include        <string.h>

#define FALSE   0
#define TRUE    (-1)

#ifndef min
#define min( x, y )     ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max( x, y )     ((x) < (y) ? (y) : (x))
#endif

#ifndef abs
#define abs(x)          ((x) > 0 ? (x) : -(x))
#endif

#ifndef sign
#define sign(x)         ((x)>0 ? 1 : (x)<0 ? -1 : 0)
#endif

class   Store;
class	ObjDictionary;

class   Object          			// root class for all
{						// OpenObjects hierarchy
public:
	int	status;				// status of an object (0,1,...)
	Object () {}
	virtual ~Object () {}


	virtual char * getClassName () const    // return object's class name
	{
		return "Object";
	}
						// whether the object confroms to given class
	virtual int     conformsTo ( const char * className ) const
	{
		return !strcmp ( className, "Object" );
	}

	virtual int     isOk () const           // whether the object is okey
	{
		return TRUE;
	}

	virtual Object * retain () const;       // clone this object
	virtual	void	 autorelease () const;	// mark object for deletion
	virtual int      put ( Store * ) const  // save object to the store
	{
		return TRUE;
	}

	virtual int     get ( Store * )         // restore object from the store
	{
		return TRUE;
	}
						// return description of current
						// status of the object
	virtual	char *	getStatusDescription ()	const
	{
		return "Ok";
	}
};

typedef Object * (*ObjectLoader)();                     // function to create object

extern	ObjDictionary	autoreleasePool;

#endif
