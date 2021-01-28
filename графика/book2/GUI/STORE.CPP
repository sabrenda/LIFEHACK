//
// Basic classes for handling stores & DOS files
//
// Author: Alex V. Boreskoff
//
#include        <malloc.h>
#include        <fcntl.h>
#include        <stdio.h>
#include        <io.h>
#include        <string.h>
#include        <sys/stat.h>

#include        "store.h"
#include        "dict.h"

struct ClassRegistryEntry
{
        char       * className;
        ObjectLoader loader;
};

int     regCompare ( const void *, const void * );

////////////////////////////////////////////////////////////////////////////

static Dictionary registry ( 20, 10, regCompare );

/////////////////////////////////////////////////////////////////////////////

int     regCompare ( const void * item1, const void * item2 )
{
        return strcmp ( ( (ClassRegistryEntry *) item1 ) -> className,
                         ( (ClassRegistryEntry *) item2 ) -> className );
}

int     registerClass ( const char * className, const ObjectLoader loader )
{
        ClassRegistryEntry * entry = new ClassRegistryEntry;

        entry -> className = (char *) className;
        entry -> loader    = (ObjectLoader) loader;

        return registry.insert ( entry );
}

////////////////////////// Store methods ////////////////////////////

char  * Store :: getString ()
{
        int     size = getInt ();

        if ( size < 0 )
                return NULL;

        if ( size == 0 )
                return "";

        char  * str = (char *) malloc ( size );

        if ( str == NULL )
                return NULL;

        return read ( str, size ) == size ? str : NULL;
}

Object * Store :: getObject ()
{
        char  * className = getString ();

        if ( className == NULL )
                return NULL;

        int     index;
        int     res = registry.find ( className, index );

        free ( className );

        if ( !res )
                return NULL;

        ObjectLoader loader = (ObjectLoader)((ClassRegistryEntry *)registry [index]) -> loader;
        Object     * obj    = loader ();

        if ( obj == NULL )
                return NULL;

        if ( !obj -> get ( this ) )
        {
                delete obj;

                return NULL;
        }

        return obj;
}

int     Store :: putObject ( Object * obj )
{
        if ( !putString ( obj -> getClassName () ) )
                return 0;

        return obj -> put ( this );
}

////////////////////////// File methods /////////////////////////////

File :: File ( const char * name, int mode )
{
        file = :: open ( strcpy ( fileName, name ), mode | O_BINARY | O_CREAT, S_IWRITE );
}

File :: ~File ()
{
        if ( file != -1 )
                :: close ( file );
}

long    File :: read ( void * ptr, long count )
{
        char  * buf = (char *) ptr;
        long    res = 0l;

        while ( count > 0 )
        {
                int     blockSize = (int) min ( 20480l, count );
                int     bytesRead = :: read ( file, buf, blockSize );

                if ( bytesRead < 0 )            // error code
                        return bytesRead;

                buf   += bytesRead;
                count -= bytesRead;
                res   += bytesRead;

                if ( bytesRead < blockSize )    // error reading
                        break;
        }

        return res;
}

long    File :: write ( void * ptr, long count )
{
        char  * buf = (char *) ptr;
        long    res = 0l;

        while ( count > 0 )
        {
                int     blockSize    = (int) min ( 20480l, count );
                int     bytesWritten = :: write ( file, (void *) buf, blockSize );

                if ( bytesWritten < 0 )
                        return bytesWritten;

                buf   += bytesWritten;
                count -= bytesWritten;
                res   += bytesWritten;

                if ( bytesWritten < blockSize )         // some error occurred
                        break;
        }

        return res;
}

long    File :: seek ( long pos )
{
        return :: lseek ( file, pos, SEEK_SET );
}

long    File :: length ()
{
        return :: filelength ( file );
}

long    File :: tell ()
{
        return :: lseek ( file, 0, SEEK_CUR );
}

long    File :: trunc ()
{
        long    pos = tell ();

        chsize ( file, pos );

        return ::filelength ( file );
}

