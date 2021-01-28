#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "resfile.h"

ResFile :: ResFile ( char * fileName, int openMode )
{
        if ( ( openMode & O_RDONLY ) == O_RDONLY )
        {
                handle = open ( fileName, O_RDONLY | O_BINARY );
                writeMode = 0;

                read ( handle, &header, sizeof ( header ) );

                resList = new ResDescriptor [header.resCount];

                lseek ( handle, header.dirOffs, SEEK_SET );
                read  ( handle, resList, header.resCount * sizeof ( ResDescriptor ) );
        }
        else
        {
                handle = open ( fileName, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE );
                writeMode = 1;

                header.signature = 0x12345678l;
                header.dirOffs   = sizeof ( header );
                header.resCount  = 0;

                lseek ( handle, sizeof header, SEEK_SET );
                resList = new ResDescriptor [2000];
        }
}

ResFile :: ~ResFile ()
{
        if ( writeMode )
        {
           write ( handle, resList, header.resCount * sizeof ( ResDescriptor ) );
           lseek ( handle, 0, SEEK_SET );
           write ( handle, &header, sizeof ( header ) );
           lseek ( handle, 0, SEEK_END );
        }

        close ( handle );

        if ( resList != NULL )
                delete [] resList;
}

int ResFile :: isOk () const
{
        return handle != -1 && resList != NULL;
}

int     ResFile :: get ( const char * id, void *& data )
{
        // find nesessary res name in res-list
	// and read this resource to allocated buffer
        if ( writeMode )
                return 0;

        for ( int i = 0; i < header.resCount; i++ )
        {
                if ( !strcmpi ( id, resList [i].name ) )
                {
                        if ( ( data = malloc ( resList [i].size ) ) == 0 )
                                return 0;

                        lseek ( handle, resList [i].offs, SEEK_SET );

                        return read ( handle, data, resList [i].size );
                }
	}

	return 0;
}

int ResFile :: put ( const char * id, void * data, long size )
{
        // cant write if read only access
        if ( !writeMode )
                return 0;

        strcpy ( resList [header.resCount].name, id );

        resList [header.resCount].offs = header.dirOffs;
        resList [header.resCount].size = size;

        int  res = write ( handle, data, size );

        header.resCount++;
        header.dirOffs += size;

        return res;
}

