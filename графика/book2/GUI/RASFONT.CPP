//
// Raster font class
// Author: Alex V. Boreskoff
//
#include        <mem.h>
#include        "store.h"
#include        "rasfont.h"

struct  FontHeader
{
        char    sign [2];
        long    width;
        long    height;
        long    baseLine;
        char    firstChar;
        char    lastChar;
        long    imageSize;
};


RasterFont :: RasterFont ( Store * st )
{
        FontHeader      hdr;

        st -> read ( &hdr, sizeof ( FontHeader ) );

        width     = hdr.width;
	height    = hdr.height;
        baseLine  = hdr.baseLine;
        firstChar = hdr.firstChar;
        lastChar  = hdr.lastChar;
        imageSize = hdr.imageSize;

        charData = new CharInfo [lastChar-firstChar+1];
        if ( charData == NULL )
                return;

        st -> read ( charData, (lastChar-firstChar+1)*sizeof (CharInfo) );

        data = malloc ( imageSize );

        if ( data == NULL )
                return;

	st -> read ( data, imageSize );
}

