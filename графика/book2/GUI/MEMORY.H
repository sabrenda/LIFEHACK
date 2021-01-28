//
// MemoryStore class
// Author: Alex V. Boreskoff
//
#ifndef	__MEMORY_STORE__
#define	__MEMORY_STORE__

#include	<malloc.h>
#include	"object.h"
#include	"store.h"

class	MemoryStore : public Store
{
	char  * pool;
	int	poolSize;
	int	poolThreshold;
	int	len;
	int	pos;
	int	shouldDelete;
public:

	MemoryStore ( int size, int threshold ) : Store ()
	{
		pool          = (char *) malloc ( size );
		poolSize      = size;
		poolThreshold = threshold;
		len           = 0;
		pos           = 0;
		shouldDelete  = TRUE;
	}

	MemoryStore ( void * buf, int size, int shouldDel = TRUE )
	{
		pool          = (char *) buf;
		poolSize      = size;
		shouldDelete  = shouldDel;
		poolThreshold = 0;
		len           = size;
		pos           = 0;
	}

	~MemoryStore ()
	{
		if ( shouldDelete )
			free ( pool );
	}

	virtual int     isOk  () const
	{
		return pool != NULL;
	}

	virtual char  * getClassName () const
	{
		return "MemoryStore";
	}

	virtual long    read  ( void *, long );
	virtual long    write ( void *, long );
	virtual long    seek  ( long to )
	{
		return pos = to;
	}

	virtual long    trunc ()
	{
		if ( pos < len )
			len = pos;

		return len;
	}

	virtual long    tell  ()
	{
		return pos;
	}

	virtual long    length ()
	{
		return len;
	}
};

#endif
