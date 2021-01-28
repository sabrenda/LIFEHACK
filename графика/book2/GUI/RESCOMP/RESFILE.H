#ifndef __RES_FILE__
#define __RES_FILE__

#include        <fcntl.h>
#include        <malloc.h>

#define RES_FILE_SIGNATURE      0x12345678L
#define RES_NAME_SIZE           10

struct  ResFileHeader
{
        long    signature;              // header signature
        long    dirOffs;                // resource directory offset in res-file
	short   resCount;               // resource count
};

struct  ResDescriptor
{
	char    name [RES_NAME_SIZE];   // resource name
	long    offs;                   // resource offset in res-file
	long    size;                   // resource size   in res-file
};

class ResFile
{
	int     handle;                 // res file handle
	int     writeMode;              // =0 for read only
					// =1 for read / write

	ResFileHeader   header;         // res file header
        ResDescriptor * resList;        // resource list pointer

public:
        ResFile ( char * fileName, int openMode = O_RDONLY );
                                        // O_RDONLY - for reading
                                        // other - for read/write
        ~ResFile ();

        int     isOk () const;                                  // return 1 in success
	int     put ( const char * id, void * data, long size );// return writed size
	int     get ( const char * id, void*& data );           // return readed size
};

#endif
