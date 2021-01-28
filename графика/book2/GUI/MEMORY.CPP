//
// MemoryStore class
// Author: Alex V. Boreskoff
//
#include	"memory.h"

long    MemoryStore :: read  ( void * buf, long count )
{
	if ( pos + count > len )
		count = len - pos;

	memcpy ( buf, pool + pos, count );

	pos += count;

	return count;
}

long    MemoryStore :: write ( void * buf, long count)
{
	if ( poolThreshold == 0 )
		return -1;

	if ( pos + count > poolSize )		// reallocate pool
	{
		poolSize = ((pos + count + poolThreshold - 1)/poolThreshold)*poolThreshold;
		pool     = (char *) realloc ( pool, poolSize );
	}

	memcpy ( pool + pos, buf, count );

	if ( ( pos += count ) > len )
		len = pos;

	return count;
}

