//
// Dictionary of objects
// Author: Alex V. Boreskoff
//
#ifndef	__OBJDICT__
#define	__OBJDICT__

#include	"dict.h"

class ObjDictionary : public Dictionary
{
public:
	ObjDictionary ( int aLimit, int aThreshold,
		     int (*compare)(const void *, const void *) = ptrCompare ) : Dictionary ( aLimit, aThreshold, compare ) {}

	virtual	void freeItem ( void * item )
	{
		delete (Object *) item;
	}
};

#endif