//
// Root class for all objects
// Author: Alex V. Boreskoff
//
#include	"object.h"
#include	"memory.h"
#include	"objdict.h"

ObjDictionary	autoreleasePool ( 10, 10 );

Object * Object :: retain () const
{
	MemoryStore  store ( 10240, 10240 );

	put ( &store );

	store.seek ( 0 );

	return store.getObject ();
}

void	Object :: autorelease () const
{
	autoreleasePool.insert ( (void *) this );
}
