//
// Basic functions for loading BMP files
// Author: Alex V. Boreskoff
//
#include        "loader.h"
#include        "store.h"

#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2
                                        // structs of BMP file
struct  BMPHeader
{
        short   type;                   // type of file, must be 'BM'
        long    size;                   // size of file in bytes
        short   reserved1, reserved2;
        long    offBits;                // offset from this header to actual data
};

struct  BMPInfoHeader
{
        long    size;                   // sizeof of this header
        long    width;                  // width of bitmap in pixels
        long    height;                 // height of bitmap in pixels
        short   planes;                 // # of planes
        short   bitCount;               // bits per pixel
        long    compression;            // type of compression, BI_RGB - no compression
        long    sizeImage;              // size of image in bytes
        long    xPelsPerMeter;          // hor. resolution of the target device
        long    yPelsPerMeter;          // vert. resolution
        long    clrUsed;
        long    clrImportant;
};

struct  RGBQuad
{
        char    blue;
        char    green;
        char    red;
        char    reserved;
};

static  BMPHeader     * hdr     = NULL;
static  BMPInfoHeader * infoHdr = NULL;
static  char          * bits    = NULL;
static  int             palette [256];
static  int             bitsSize;
static  int             numColors;
static  int             width;
static  int             height;
static  int             bitsPos;

inline  int     getByte ()
{
        return bitsPos < bitsSize ? bits [bitsPos++] : -1;
}

static  int     loadRGB4  ( Surface * );
static  int     loadRGB8  ( Surface * );
static  int     loadRGB24 ( Surface * );
static  int     loadRLE4  ( Surface * );
static  int     loadRLE8  ( Surface * );

Surface * loadBMP ( const void * data, int size, const Surface& surface, int& code )
{
        hdr     = (BMPHeader *) data;
        infoHdr = (BMPInfoHeader *)(hdr + 1);

        code =  hdr -> type == 0x4D42 && infoHdr -> size == 40 && infoHdr -> planes == 1 &&
               ( infoHdr -> bitCount == 1 || infoHdr -> bitCount == 2 ||
                 infoHdr -> bitCount == 4 || infoHdr -> bitCount == 8 || infoHdr -> bitCount == 24 ) &&
               ( infoHdr -> compression == BI_RGB || infoHdr -> compression == BI_RLE4 || infoHdr -> compression == BI_RLE8 );

        if ( !code )                    // bad header data
        {
                code = LE_WRONGFORMAT;

                return NULL;
        }

        numColors = 1 << infoHdr -> bitCount;
        width     = infoHdr      -> width;
        height    = infoHdr      -> height;
        bits      = hdr          -> offBits + (char *) data;
        bitsSize  = size -   hdr -> offBits;
        bitsPos   = 0;

        RGBQuad * pal = (RGBQuad *)(infoHdr + 1);

        for ( int i = 0; i < numColors; i++ )
        {
                RGB     color ( pal [i].red, pal [i].green, pal [i].blue );

                palette [i] = surface.closestColor ( color );
        }

        Surface * surf = new Surface ( width, height, surface.format );

        if ( surf == NULL )
                return NULL;

        if ( !surf -> isOk () )
        {
                delete surf;

                return NULL;
        }

        surf -> beginDraw ();

        if ( infoHdr -> compression == BI_RGB )
        {
                if ( infoHdr -> bitCount == 4 ) // 16-colors uncompressed
                        code = loadRGB4 ( surf );
                else
                if ( infoHdr -> bitCount == 8 ) // 256-colors uncompressed
                        code = loadRGB8 ( surf );
                else
                if ( infoHdr -> bitCount == 24 )// True-Color bitmap
                        code = loadRGB24 ( surf );
        }
        else
        if ( infoHdr -> compression == BI_RLE4 )// 16-colors RLE compressed
                code = loadRLE4 ( surf );
        else
        if ( infoHdr -> compression == BI_RLE8 )// 256-colors RLE compressed
                code = loadRLE8 ( surf );
        else
                code = LE_WRONGFORMAT;

        surf -> endDraw ();

        if ( code != LE_OK )
        {
                delete surf;

                return NULL;
        }

        return surf;
}

int     loadRGB4 ( Surface * surface )
{
        int     count, byte;

        for ( int y = height - 1; y >= 0; y-- )
        {
                for ( int x = 0; x < width; x++ )
                        if ( ( x & 1 ) == 0 )
                                if ( ( byte = getByte () ) == -1 )
                                        return LE_DATAERROR;
                                else
                                        surface -> drawPixel ( x, y, palette [byte >> 4] );
                        else
                                surface -> drawPixel ( x, y, palette [byte & 0x0F] );

                for ( count = ( width + 1 ) / 2; count % 4; count++ )
                        if ( getByte () == -1 )
                                return LE_DATAERROR;
        }

        return LE_OK;
}

int     loadRGB8 ( Surface * surface )
{
        int     count, byte;

        for ( int y = height - 1; y >= 0; y-- )
        {
                for ( int x = count = 0; x < width; x++, count++ )
                        if ( ( byte = getByte () ) == -1 )
                                return LE_DATAERROR;
                        else
                                surface -> drawPixel ( x, y, palette [byte] );

                for ( ; count % 4; count++ )    // skip remaining bytes
                        if ( getByte () == -1 )
                                return LE_DATAERROR;
        }

        return LE_OK;
}

int     loadRGB24 ( Surface * surface )
{
        int     count;
        int     red, green, blue;

        for ( int y = height - 1; y >= 0; y-- )
        {
                for ( int x = count = 0; x < width; x++, count += 3 )
                {
                        if ( ( red = getByte () ) == -1 )
                                return LE_DATAERROR;

                        if ( ( green = getByte () ) == -1 )
                                return LE_DATAERROR;

                        if ( ( blue = getByte () ) == -1 )
                                return LE_DATAERROR;

                        surface -> drawPixel ( x, y, rgbTo24Bit ( red, green, blue ) );
                }

                for ( ; count % 4; count++ )    // skip remaining bytes
                        if ( getByte () == -1 )
                                return LE_DATAERROR;
        }

        return LE_OK;
}

int     loadRLE4 ( Surface * surface )
{
        int     y = height - 1;
        int     x = 0;
        int     count, byte;

        for ( ; ; )
        {
                if ( ( count = getByte () ) == -1 )
                        return LE_DATAERROR;
                else
                if ( count == 0 )
                {
                        if ( ( count = getByte () ) == -1 )
                                return LE_DATAERROR;
                        else
                        if ( count == 0 )       // end of line
                        {
                                y--;
                                x = 0;
                        }
                        else
                        if ( count == 1 )       // 0, 1 == end of RLE data
                                break;
                        else
                        if ( count == 2 )       // 0, 2 == delta record
                        {
                                y -= getByte ();
                                x += getByte ();
                        }
                        else                    // start of an unencoded block
                        {
                                for ( int i = 0; i < count; i += 2, x += 2 )
                                {
                                        if ( ( byte = getByte () ) == -1 )
                                                return LE_DATAERROR;

                                        surface -> drawPixel ( x,     y, palette [byte >> 4] );
                                        surface -> drawPixel ( x + 1, y, palette [byte & 0x0F] );
                                }

                                if ( ( count / 2 ) & 1 )
                                        if ( getByte () == -1 )
                                                return LE_DATAERROR;
                        }
                }
                else                    // RLE decoded record
                {
                        if ( ( byte = getByte () ) == -1 )
                                return LE_DATAERROR;

                        for ( int i = 0; i < count; i++, x++ )
                                if ( i & 1 )
                                        surface -> drawPixel ( x, y, palette [byte & 0x0F] );
                                else
                                        surface -> drawPixel ( x, y, palette [byte >> 4] );
                }
        }

        return LE_OK;
}

int     loadRLE8 ( Surface * surface )
{
        int     y = height - 1;
        int     x = 0;
        int     count, byte;

        for ( ; ; )
        {
                if ( ( count = getByte () ) == -1 )
                        return LE_DATAERROR;
                else
                if ( count == 0 )
                {
                        if ( ( count = getByte () ) == -1 )
                                return LE_DATAERROR;
                        else
                        if ( count == 0 )       // end of line
                        {
                                y--;
                                x = 0;
                        }
                        else
                        if ( count == 1 )       // 0, 1 == end of RLE data
                                break;
                        else
                        if ( count == 2 )       // 0, 2 == delta record
                        {
                                y -= getByte ();
                                x += getByte ();
                        }
                        else                    // start of an unencoded block
                        {
                                for ( int i = 0; i < count; i++, x ++ )
                                        if ( ( byte = getByte () ) == -1 )
                                                return LE_DATAERROR;
                                        else
                                                surface -> drawPixel ( x, y, palette [byte] );

                                if ( count & 1 )
                                        if ( getByte () == -1 )
                                                return LE_DATAERROR;
                        }
                }
                else                    // RLE decoded record
                {
                        if ( ( byte = getByte () ) == -1 )
                                return LE_DATAERROR;

                        for ( int i = 0; i < count; i++, x++ )
                                surface -> drawPixel ( x, y, palette [byte] );
                }
        }

        return LE_OK;
}

int     saveBMP ( const Surface * surface, Store * s )
{
        if ( ! s -> isOk () )
                return FALSE;

        if ( ! surface -> isOk () )
                return FALSE;

        BMPHeader       bmpHeader;
        BMPInfoHeader   infoHeader;

        bmpHeader.type         = 0x4D42;                // "BM"
        bmpHeader.size         = sizeof (bmpHeader) + sizeof ( infoHeader );
        bmpHeader.reserved1    = 0;
        bmpHeader.reserved2    = 0;
        bmpHeader.offBits      = sizeof (bmpHeader) + sizeof ( infoHeader ) + ( surface -> format.bytesPerPixel == 1 ? 256*sizeof (RGBQuad) : 0 );

        infoHeader.size        = 40;
        infoHeader.width       = surface -> width;
        infoHeader.height      = surface -> height;
        infoHeader.planes      = 1;
        infoHeader.bitCount    = surface -> format.bytesPerPixel == 1 ? 8 : 24;
        infoHeader.compression = BI_RGB;
        infoHeader.sizeImage   = 0;

        return TRUE;
}
