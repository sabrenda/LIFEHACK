//
// Generic file class Store ( since Stream was already used )
// all file operations should be done through its descendants
// Supports all major file operations - read, write, seek, tell, trunc
//
// Author: Alex V. Boreskoff
// Last modification: 25/01/99
//
#ifndef __STORE__
#define __STORE__

#include        <fcntl.h>
#include        <string.h>

#include        "object.h"

#define MAXSHORT    0x7FFF
#define MAXINT  0x7FFFFFFF
#define MAXDOUBLE   1.797693E+308
#define MAXFLOAT    3.37E+38

class   Store : public Object
{
public:
        virtual int     isOk  () const
        {
                return FALSE;           // should be overridden
        }

        virtual char  * getClassName () const
        {
                return "Store";
        }

        virtual long    read  ( void *, long ) = 0;
        virtual long    write ( void *, long ) = 0;
        virtual long    seek  ( long ) = 0;
        virtual long    trunc ()       = 0;
        virtual long    tell  ()       = 0;
        virtual long    length ()      = 0;

        int     getChar ()
        {
                char    ch;

                if ( read ( &ch, sizeof ( char ) ) != sizeof ( char ) )
                        return -1;

                return (int) ch;
        }

        int     putChar ( int ch )
        {
                return write ( &ch, sizeof ( char ) ) == sizeof ( char );
        }

        int     getShort ()
        {
                short   v;

                if ( read ( &v, sizeof ( short ) ) != sizeof ( short ) )
                        return -1;

                return (int) v;
        }

        int     getInt ()
        {
                int     v;

                if ( read ( &v, sizeof ( int ) ) != sizeof ( int ) )
                        return -1;

                return v;
        }

        int     putInt ( int v )
        {
                return write ( &v, sizeof ( int ) ) == sizeof ( int );
        }

        int     putShort ( int v )
        {
                return write ( &v, sizeof ( short ) ) == sizeof ( short );
        }

        long    getLong ()
        {
                long    v;

                if ( read ( &v, sizeof ( long ) ) != sizeof ( long ) )
                        return -1;

                return v;
        }

        int     putLong ( long v )
        {
                return write ( &v, sizeof ( long ) ) == sizeof ( long );
        }

        float   getFloat ()
        {
                float   v;

                if ( read ( &v, sizeof ( float ) ) != sizeof ( float ) )
                        return MAXFLOAT;

                return v;
        }

        int     putFloat ( float v )
        {
                return write ( &v, sizeof ( float ) ) == sizeof ( float );
        }

        char  * getString ();

        int     putString ( char * str )
        {
                int     size = strlen ( str ) + 1;

                if ( putInt ( size ) == -1 )
                        return 0;

                return write ( str, size ) == size;
        }

        Object * getObject ();

        int      putObject ( Object * );
};

class   File : public Store                     // simple OS file
{
public:
        int     file;                           // file descriptor
        char    fileName [128];                 // file name

	File ( const char * name, int mode = O_RDWR );
        ~File ();

        virtual char  * getClassName () const
        {
                return "File";
        }

        virtual int     isOk   () const
        {
                return file != -1;
        }

        virtual long    read   ( void *, long );
        virtual long    write  ( void *, long );
        virtual long    seek   ( long );
        virtual long    trunc  ();
        virtual long    tell   ();
        virtual long    length ();
};

int     registerClass ( const char * className, const ObjectLoader loader );

#endif
