//
// Basic class for images and all 2D graphics
// You can add your own functions
// currently supports only 8-bit and 15/16-bit modes
//
// Author: Alex V. Boreskoff
// Last change: 01/02/1999
//
#include        <mem.h>
#include        "surface.h"
#include        "store.h"

#define CLIP_LEFT       0x01
#define CLIP_TOP        0x02
#define CLIP_RIGHT      0x04
#define CLIP_BOTTOM     0x04

void    PixelFormat :: completeFromMasks ()
{
        register unsigned int     s;

        redShift = 0;
        redBits  = 0;

        for ( s = redMask; s && !(s&1); s >>= 1 )
                redShift++;

        for ( s = redMask >> redShift; s; s >>= 1 )
                redBits++;

        greenShift = 0;
        greenBits  = 0;

        for ( s = greenMask; s && !(s&1); s >>= 1 )
                greenShift++;


        for ( s = greenMask >> greenShift; s; s >>= 1 )
                greenBits++;

        blueShift = 0;
        blueBits  = 0;

        for ( s = blueMask; s && !(s&1); s >>= 1 )
                blueShift++;

        for ( s = blueMask >> blueShift; s; s >>= 1 )
                blueBits++;

        bitsPerPixel  = redBits + greenBits + blueBits;
        bytesPerPixel = (bitsPerPixel + 7) >> 3;
}

inline  int     outCodes ( int x, int y, const Rect& r )
{
        int     u = 0;

        if ( x < r.x1 )
                u |= CLIP_LEFT;

        if ( y < r.y1 )
                u |= CLIP_TOP;

        if ( x > r.x2 )
                u |= CLIP_RIGHT;

        if ( y > r.y2 )
                u |= CLIP_BOTTOM;

        return u;
}


Surface :: Surface ( int w, int h, const PixelFormat& fmt ) : Object ()
{
        format = fmt;

        width            = w;
        height           = h;
        format           = fmt;
        bytesPerScanLine = width * fmt.bytesPerPixel;
        data             = malloc ( height * bytesPerScanLine );
        palette          = NULL;
        curFont          = NULL;
        rasterOp         = RO_COPY;
        clipRect.x1      = 0;
        clipRect.y1      = 0;
        clipRect.x2      = width - 1;
        clipRect.y2      = height - 1;
        org.x            = 0;
        org.y            = 0;

        setFuncPointers ();

        if ( data != NULL )
                memset ( data, '\0', height * bytesPerScanLine );
}

int     Surface :: put ( Store * s ) const
{
        s -> write  ( (void *) &format, sizeof ( format ) );
        s -> putInt ( width );
        s -> putInt ( height );

        if ( format.bytesPerPixel == 1 )
                s -> write ( palette, 256 * sizeof ( RGB ) );

        s -> write  ( data, height * bytesPerScanLine );

        return TRUE;
}

int     Surface :: get ( Store * s )
{
        if ( data != NULL )
                free ( data );

        if ( palette != NULL )
                delete [] palette;

        s -> read ( &format, sizeof ( format ) );

        width  = s -> getInt ();
        height = s -> getInt ();

        bytesPerScanLine = width * format.bytesPerPixel;
        data             = malloc ( height * bytesPerScanLine );
        palette          = NULL;
        curFont          = NULL;
        rasterOp         = RO_COPY;
        clipRect.x1      = 0;
        clipRect.y1      = 0;
        clipRect.x2      = width - 1;
        clipRect.y2      = height - 1;
        org.x            = 0;
        org.y            = 0;

        if ( format.bytesPerPixel == 1 )
        {
                palette = new RGB [256];

                s -> read ( palette, 256 * sizeof ( RGB ) );
        }

        if ( data != NULL )
        {
                memset ( data, '\0', height * bytesPerScanLine );

                s -> read ( data, height * bytesPerScanLine );
        }
        else
                return FALSE;

        return TRUE;
}

void    Surface :: setFuncPointers ()
{
        if ( format.bytesPerPixel == 1 )
        {
                getPixelAddr   = & Surface :: getPixel8;
                drawPixelAddr  = & Surface :: drawPixel8;
                drawLineAddr   = & Surface :: drawLine8;
                drawCharAddr   = & Surface :: drawChar8;
                drawStringAddr = & Surface :: drawString8;
                drawBarAddr    = & Surface :: drawBar8;
                copyAddr       = & Surface :: copy8;
                copyTranspAddr = & Surface :: copyTransp8;
        }
        else
        if ( format.bytesPerPixel == 2 )
        {
                getPixelAddr   = & Surface :: getPixel16;
                drawPixelAddr  = & Surface :: drawPixel16;
                drawLineAddr   = & Surface :: drawLine16;
                drawCharAddr   = & Surface :: drawChar16;
                drawStringAddr = & Surface :: drawString16;
                drawBarAddr    = & Surface :: drawBar16;
                copyAddr       = & Surface :: copy16;
                copyTranspAddr = & Surface :: copyTransp16;
        }
}

int     Surface :: closestColor ( const RGB& color ) const
{
        if ( format.bytesPerPixel > 1 )
                return rgbToInt ( color.red, color.green, color.blue, format );

        int     dist = 1000;
        int     index;

        if ( palette == NULL )
                return 0;

        for ( register int i = 0; i < 256; i++ )
        {
                int     d = abs (palette [i].red   - color.red) +
                            abs (palette [i].green - color.green) +
                            abs (palette [i].blue  - color.blue);

                if ( d < dist )
                {
                        index = i;
                        dist  = d;
                }
        }

        return index;
}

void    Surface :: drawFrame ( const Rect& r )
{
        drawLine ( r.x1, r.y1, r.x2, r.y1 );
        drawLine ( r.x2, r.y1, r.x2, r.y2 );
        drawLine ( r.x2, r.y2, r.x1, r.y2 );
        drawLine ( r.x1, r.y2, r.x1, r.y1 );
}


void    Surface :: drawPixel8 ( int x, int y, int color )
{
        x += org.x;
        y += org.x;

        if ( clipRect.contains ( x, y ) )
                switch ( rasterOp )
                {
                        case RO_COPY:
                                * (char *) pixelAddr ( x, y ) = (char) color;
                                break;

                        case RO_OR:
                                * (char *) pixelAddr ( x, y ) |= (char) color;
                                break;

                        case RO_AND:
                                * (char *) pixelAddr ( x, y ) &= (char) color;
                                break;

                        case RO_XOR:
                                * (char *) pixelAddr ( x, y ) ^= (char) color;
                }
}

void    Surface :: drawPixel16 ( int x, int y, int color )
{
        x += org.x;
        y += org.x;

        if ( clipRect.contains ( x, y ) )
                switch ( rasterOp )
                {
                        case RO_COPY:
                                * (short *) pixelAddr ( x, y ) = (short) color;
                                break;

                        case RO_OR:
                                * (short *) pixelAddr ( x, y ) |= (short) color;
                                break;

                        case RO_AND:
                                * (short *) pixelAddr ( x, y ) &= (short) color;
                                break;

                        case RO_XOR:
                                * (short *) pixelAddr ( x, y ) ^= (short) color;
                }
}

int     Surface :: getPixel8   ( int x,  int y )
{
        return * (unsigned char *) pixelAddr ( org.x + x, org.y + y );
}

int     Surface :: getPixel16   ( int x,  int y )
{
        return * (short *) pixelAddr ( org.x + x, org.y + y );
}

void    Surface :: drawLine8   ( int x1, int y1, int x2, int y2 )
{
        x1 += org.x;
        y1 += org.y;
        x2 += org.x;
        y2 += org.y;

        if ( x1 == x2 )         // vertical line
        {
                if ( x1 < clipRect.x1 || x1 > clipRect.x2 )
                        return;

                if ( y1 > y2 )
                {
                        int     tmp = y1;

                        y1 = y2;
                        y2 = tmp;
                }

                if ( y1 < clipRect.y1 )
                        y1 = clipRect.y1;

                if ( y2 > clipRect.y2 )
                        y2 = clipRect.y2;

                register char * addr = (char *) pixelAddr ( x1, y1 );

                if ( rasterOp == RO_COPY )
                        for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine )
                                * addr = (char) color;
                else
                if ( rasterOp == RO_OR )
                        for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine )
                                * addr |= (char) color;
                else
                if ( rasterOp == RO_AND )
                        for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine )
                                * addr &= (char) color;
                else
                if ( rasterOp == RO_XOR )
                        for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine )
                                * addr ^= (char) color;

                return;
        }

        if ( y1 == y2 )         // horizontal line
        {
                if ( y1 < clipRect.y1 || y1 > clipRect.y2 )
                        return;

                if ( x1 > x2 )
                {
                        int     tmp = x1;

                        x1 = x2;
                        x2 = tmp;
                }

                if ( x1 < clipRect.x1 )
                        x1 = clipRect.x1;

                if ( x2 > clipRect.x2 )
                        x2 = clipRect.x2;

                register char * addr = (char *) pixelAddr ( x1, y1 );

                if ( rasterOp == RO_COPY )
                        for ( register int x = x1; x <= x2; x++, addr++ )
                                * addr = (char) color;
                else
                if ( rasterOp == RO_OR )
                        for ( register int x = x1; x <= x2; x++, addr++ )
                                * addr |= (char) color;
                else
                if ( rasterOp == RO_AND )
                        for ( register int x = x1; x <= x2; x++, addr++ )
                                * addr &= (char) color;
                else
                if ( rasterOp == RO_XOR )
                        for ( register int x = x1; x <= x2; x++, addr++ )
                                * addr ^= (char) color;

                return;
        }

        int     ocu1    = outCodes ( x1, y1, clipRect );
        int     ocu2    = outCodes ( x2, y2, clipRect );
        int     inside  = ( ocu1 | ocu2 ) == 0;
        int     outside = ( ocu1 & ocu2 ) != 0;

        while ( (!outside) && (!inside) )
        {
                if ( ocu1 == 0 )        // swap ax1, ay1, ocu1 with ax2, ay2, ocu2
                {
                        int     tmp;

                        tmp = x1;     x1 = x2;    x2  = tmp;
                        tmp = y1;     y1 = y2;    y2  = tmp;
                        tmp = ocu1; ocu1 = ocu2; ocu2 = tmp;
                }

                if ( ocu1 & CLIP_LEFT )
                {
                        y1 += ((y2 - y1)*(clipRect.x1 - x1)) / (x2 - x1);
                        x1  = clipRect.x1;
                }
                else
                if ( ocu1 & CLIP_TOP )
                {
                        x1 += ((x2 - x1)*(clipRect.y1 - y1)) / (y2 - y1);
                        y1  = clipRect.y1;
                }
                else
                if ( ocu1 & CLIP_RIGHT )
                {
                        y1 += ((y2 - y1)*(clipRect.x2 - x1)) / (x2 - x1);
                        x1  = clipRect.x2;
                }
                else
                if ( ocu1 & CLIP_BOTTOM )
                {
                        x1 += ((x2 - x1)*(clipRect.y2 - y1)) / (x2 - x1);
                        y1  = clipRect.y2;
                }

                ocu1    = outCodes ( x1, y1, clipRect );
                inside  = (ocu1 | ocu2) == 0;
                outside = (ocu1 & ocu2) != 0;
        }

        if ( outside )          // check whether smth remains in current rect
                return;
                                // now do draw line (x1,y1)-(x2,y2)
        int              dx   = abs ( x2 - x1 );
        int              dy   = abs ( y2 - y1 );
        int              sx   = x2 >= x1 ? 1 : -1;
        int              sy   = y2 >= y1 ? 1 : -1;
        register char  * addr = (char *) pixelAddr ( x1, y1 );

        if ( rasterOp == RO_COPY )
        {
                if ( dy <= dx )
                {
                        int     d  = ( dy << 1 ) - dx;
                        int     d1 = dy << 1;
                        int     d2 = ( dy - dx ) << 1;

                        * addr = (char) color;
                        addr  += sx;

                        for ( register int i = 1; i <= dx; i++, addr += sx )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += bytesPerScanLine;
                                }
                                else
                                        d += d1;

                                * addr = (char) color;
                        }
                }
                else
                {
                        int     d  = ( dx << 1 ) - dy;
                        int     d1 = dx << 1;
                        int     d2 = ( dx - dy ) << 1;

                        * addr = (char) color;

                        if ( sy > 0 )
                                sy = bytesPerScanLine;
                        else
                                sy = -bytesPerScanLine;

                        addr += sy;

                        for ( register int i = 1; i <= dy; i++, addr += sy )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += sx;
                                }
                                else
                                        d += d1;

                                * addr = (char) color;
                        }
                }
        }
        else
        if ( rasterOp == RO_OR )
        {
                if ( dy <= dx )
                {
                        int     d  = ( dy << 1 ) - dx;
                        int     d1 = dy << 1;
                        int     d2 = ( dy - dx ) << 1;

                        * addr |= (char) color;
                        addr  += sx;

                        for ( register int i = 1; i <= dx; i++, addr += sx )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += bytesPerScanLine;
                                }
                                else
                                        d += d1;

                                * addr |= (char) color;
                        }
                }
                else
                {
                        int     d  = ( dx << 1 ) - dy;
                        int     d1 = dx << 1;
                        int     d2 = ( dx - dy ) << 1;

                        * addr |= (char) color;

                        if ( sy > 0 )
                                sy = bytesPerScanLine;
                        else
                                sy = -bytesPerScanLine;

                        addr += sy;

                        for ( register int i = 1; i <= dy; i++, addr += sy )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += sx;
                                }
                                else
                                        d += d1;

                                * addr |= (char) color;
                        }
                }
        }
        else
        if ( rasterOp == RO_AND )
        {
                if ( dy <= dx )
                {
                        int     d  = ( dy << 1 ) - dx;
                        int     d1 = dy << 1;
                        int     d2 = ( dy - dx ) << 1;

                        * addr &= (char) color;
                        addr  += sx;

                        for ( register int i = 1; i <= dx; i++, addr += sx )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += bytesPerScanLine;
                                }
                                else
                                        d += d1;

                                * addr &= (char) color;
                        }
                }
                else
                {
                        int     d  = ( dx << 1 ) - dy;
                        int     d1 = dx << 1;
                        int     d2 = ( dx - dy ) << 1;

                        * addr &= (char) color;

                        if ( sy > 0 )
                                sy = bytesPerScanLine;
                        else
                                sy = -bytesPerScanLine;

                        addr += sy;

                        for ( register int i = 1; i <= dy; i++, addr += sy )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += sx;
                                }
                                else
                                        d += d1;

                                * addr &= (char) color;
                        }
                }
        }
        else
        if ( rasterOp == RO_XOR )
        {
                if ( dy <= dx )
                {
                        int     d  = ( dy << 1 ) - dx;
                        int     d1 = dy << 1;
                        int     d2 = ( dy - dx ) << 1;

                        * addr ^= (char) color;
                        addr  += sx;

                        for ( register int i = 1; i <= dx; i++, addr += sx )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += bytesPerScanLine;
                                }
                                else
                                        d += d1;

                                * addr ^= (char) color;
                        }
                }
                else
                {
                        int     d  = ( dx << 1 ) - dy;
                        int     d1 = dx << 1;
                        int     d2 = ( dx - dy ) << 1;

                        * addr ^= (char) color;

                        if ( sy > 0 )
                                sy = bytesPerScanLine;
                        else
                                sy = -bytesPerScanLine;

                        addr += sy;

                        for ( register int i = 1; i <= dy; i++, addr += sy )
                        {
                                if ( d > 0 )
                                {
                                        d    += d2;
                                        addr += sx;
                                }
                                else
                                        d += d1;

                                * addr ^= (char) color;
                        }
                }
        }
}

void    Surface :: drawLine16   ( int x1, int y1, int x2, int y2 )
{
        x1 += org.x;
        y1 += org.y;
        x2 += org.x;
        y2 += org.y;

        if ( x1 == x2 )         // vertical line
        {
                if ( x1 < clipRect.x1 || x1 > clipRect.x2 )
                        return;

                if ( y1 > y2 )
                {
                        int     tmp = y1;

                        y1 = y2;
                        y2 = tmp;
                }

                if ( y1 < clipRect.y1 )
                        y1 = clipRect.y1;

                if ( y2 > clipRect.y2 )
                        y2 = clipRect.y2;

                register short * addr = (short *) pixelAddr ( x1, y1 );

                if ( rasterOp == RO_COPY )
			for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine >> 1 )
				* addr = (short) color;
		else
		if ( rasterOp == RO_OR )
			for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine >> 1 )
				* addr |= (short) color;
		else
		if ( rasterOp == RO_AND )
			for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine >> 1 )
				* addr &= (short) color;
		else
		if ( rasterOp == RO_XOR )
			for ( register int y = y1; y <= y2; y++, addr += bytesPerScanLine >> 1 )
				* addr ^= (short) color;

		return;
	}

	if ( y1 == y2 )         // horizontal line
	{
		if ( y1 < clipRect.y1 || y1 > clipRect.y2 )
			return;

		if ( x1 > x2 )
		{
			int     tmp = x1;

			x1 = x2;
			x2 = tmp;
		}

		if ( x1 < clipRect.x1 )
			x1 = clipRect.x1;

		if ( x2 > clipRect.x2 )
			x2 = clipRect.x2;

		register short * addr = (short *) pixelAddr ( x1, y1 );

		if ( rasterOp == RO_COPY )
			for ( register int x = x1; x <= x2; x++, addr++ )
				* addr = (short) color;
		else
		if ( rasterOp == RO_OR )
			for ( register int x = x1; x <= x2; x++, addr++ )
				* addr |= (short) color;
		else
		if ( rasterOp == RO_AND )
			for ( register int x = x1; x <= x2; x++, addr++ )
				* addr &= (short) color;
		else
		if ( rasterOp == RO_XOR )
			for ( register int x = x1; x <= x2; x++, addr++ )
				* addr ^= (short) color;

		return;
	}

	int     ocu1    = outCodes ( x1, y1, clipRect );
	int     ocu2    = outCodes ( x2, y2, clipRect );
	int     inside  = ( ocu1 | ocu2 ) == 0;
	int     outside = ( ocu1 & ocu2 ) != 0;

	while ( (!outside) && (!inside) )
	{
		if ( ocu1 == 0 )        // swap ax1, ay1, ocu1 with ax2, ay2, ocu2
		{
			int     tmp;

			tmp = x1;     x1 = x2;    x2  = tmp;
			tmp = y1;     y1 = y2;    y2  = tmp;
			tmp = ocu1; ocu1 = ocu2; ocu2 = tmp;
		}

		if ( ocu1 & CLIP_LEFT )
		{
			y1 += ((y2 - y1)*(clipRect.x1 - x1)) / (x2 - x1);
			x1  = clipRect.x1;
		}
		else
		if ( ocu1 & CLIP_TOP )
		{
			x1 += ((x2 - x1)*(clipRect.y1 - y1)) / (y2 - y1);
			y1  = clipRect.y1;
		}
		else
		if ( ocu1 & CLIP_RIGHT )
		{
			y1 += ((y2 - y1)*(clipRect.x2 - x1)) / (x2 - x1);
			x1  = clipRect.x2;
		}
		else
		if ( ocu1 & CLIP_BOTTOM )
		{
			x1 += ((x2 - x1)*(clipRect.y2 - y1)) / (x2 - x1);
			y1  = clipRect.y2;
		}

		ocu1    = outCodes ( x1, y1, clipRect );
		inside  = (ocu1 | ocu2) == 0;
		outside = (ocu1 & ocu2) != 0;
	}

	if ( outside )          // check whether smth remains in current rect
		return;
				// now do draw line (x1,y1)-(x2,y2)
	int              dx   = abs ( x2 - x1 );
	int              dy   = abs ( y2 - y1 );
	int              sx   = x2 >= x1 ? 1 : -1;
	int              sy   = y2 >= y1 ? 1 : -1;
	register short  * addr = (short *) pixelAddr ( x1, y1 );

	if ( rasterOp == RO_COPY )
	{
		if ( dy <= dx )
		{
			int     d  = ( dy << 1 ) - dx;
			int     d1 = dy << 1;
			int     d2 = ( dy - dx ) << 1;

			* addr = (short) color;
			addr  += sx;

			for ( register int i = 1; i <= dx; i++, addr += sx )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += bytesPerScanLine >> 1;
				}
				else
					d += d1;

				* addr = (short) color;
			}
		}
		else
		{
			int     d  = ( dx << 1 ) - dy;
			int     d1 = dx << 1;
			int     d2 = ( dx - dy ) << 1;

			* addr = (short) color;

			if ( sy > 0 )
				sy = bytesPerScanLine >> 1;
			else
				sy = -(bytesPerScanLine >> 1);

			addr += sy;

			for ( register int i = 1; i <= dy; i++, addr += sy )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += sx;
				}
				else
					d += d1;

				* addr = (short) color;
			}
		}
	}
	else
	if ( rasterOp == RO_OR )
	{
		if ( dy <= dx )
		{
			int     d  = ( dy << 1 ) - dx;
			int     d1 = dy << 1;
			int     d2 = ( dy - dx ) << 1;

			* addr |= (short) color;
			addr  += sx;

			for ( register int i = 1; i <= dx; i++, addr += sx )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += bytesPerScanLine >> 1;
				}
				else
					d += d1;

				* addr |= (short) color;
			}
		}
		else
		{
			int     d  = ( dx << 1 ) - dy;
			int     d1 = dx << 1;
			int     d2 = ( dx - dy ) << 1;

			* addr |= (short) color;

			if ( sy > 0 )
				sy = bytesPerScanLine >> 1;
			else
				sy = -(bytesPerScanLine >> 1);

			addr += sy;

			for ( register int i = 1; i <= dy; i++, addr += sy )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += sx;
				}
				else
					d += d1;

				* addr |= (short) color;
			}
		}
	}
	else
	if ( rasterOp == RO_AND )
	{
		if ( dy <= dx )
		{
			int     d  = ( dy << 1 ) - dx;
			int     d1 = dy << 1;
			int     d2 = ( dy - dx ) << 1;

			* addr &= (short) color;
			addr  += sx;

			for ( register int i = 1; i <= dx; i++, addr += sx )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += bytesPerScanLine >> 1;
				}
				else
					d += d1;

				* addr &= (short) color;
			}
		}
		else
		{
			int     d  = ( dx << 1 ) - dy;
			int     d1 = dx << 1;
			int     d2 = ( dx - dy ) << 1;

			* addr &= (short) color;

			if ( sy > 0 )
				sy = bytesPerScanLine >> 1;
			else
				sy = - (bytesPerScanLine >> 1);

			addr += sy;

			for ( register int i = 1; i <= dy; i++, addr += sy )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += sx;
				}
				else
					d += d1;

				* addr &= (short) color;
			}
		}
	}
	else
	if ( rasterOp == RO_XOR )
	{
		if ( dy <= dx )
		{
			int     d  = ( dy << 1 ) - dx;
			int     d1 = dy << 1;
			int     d2 = ( dy - dx ) << 1;

			* addr ^= (short) color;
			addr  += sx;

			for ( register int i = 1; i <= dx; i++, addr += sx )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += bytesPerScanLine >> 1;
				}
				else
					d += d1;

				* addr ^= (short) color;
			}
		}
		else
		{
			int     d  = ( dx << 1 ) - dy;
			int     d1 = dx << 1;
			int     d2 = ( dx - dy ) << 1;

			* addr ^= (short) color;

			if ( sy > 0 )
				sy = bytesPerScanLine >> 1;
			else
				sy = - (bytesPerScanLine >> 1);

			addr += sy;

			for ( register int i = 1; i <= dy; i++, addr += sy )
			{
				if ( d > 0 )
				{
					d    += d2;
					addr += sx;
				}
				else
					d += d1;

				* addr ^= (short) color;
			}
		}
	}
}

void    Surface :: drawChar8   ( int x,  int y, int ch )
{
	x += org.x;
	y += org.y;

	if ( curFont == NULL || x > clipRect.x2 || y > clipRect.y2 )
		return;

	if ( ch < curFont -> firstChar || ch >= curFont -> lastChar )
		return;

	int     charWidth  = curFont -> charWidth ( ch );
	int     charHeight = curFont -> height;

	if ( x + charWidth < clipRect.x1 || y + charHeight < clipRect.y1 )
		return;

	register char  * image             = (char *) curFont -> charImage ( ch );
	register char  * addr              = (char *) pixelAddr ( x, y );
        int              imageBytesPerLine = (charWidth+7) >> 3;
        int              lx                = x < clipRect.x1 ? clipRect.x1 - x : 0;

        if ( x + charWidth > clipRect.x2 )
                charWidth = clipRect.x2 - x;

        for ( register int i = 0; i < charHeight; i++, addr += bytesPerScanLine, image += imageBytesPerLine )
        {
                if ( y + i < clipRect.y1 )
                        continue;

                if ( y + i > clipRect.y2 )
                        break;

                if ( rasterOp == RO_COPY )
                {
                        for ( register int j = lx; j < charWidth; j++ )
                                if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
                                        addr [j] = (char) color;
		}
                else
                if ( rasterOp == RO_OR )
                {
                        for ( register int j = lx; j < charWidth; j++ )
                                if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
                                        addr [j] |= (char) color;
                }
                else
                if ( rasterOp == RO_AND )
                {
                        for ( register int j = lx; j < charWidth; j++ )
                                if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
                                        addr [j] &= (char) color;
                }
                else
                if ( rasterOp == RO_XOR )
                {
                        for ( register int j = lx; j < charWidth; j++ )
                                if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
					addr [j] ^= (char) color;
                }
        }
}

void    Surface :: drawChar16   ( int x,  int y, int ch )
{
        x += org.x;
        y += org.y;

        if ( curFont == NULL || x > clipRect.x2 || y > clipRect.y2 )
                return;

        if ( ch < curFont -> firstChar || ch >= curFont -> lastChar )
                return;

        int     charWidth  = curFont -> charWidth ( ch );
	int     charHeight = curFont -> height;

	if ( x + charWidth < clipRect.x1 || y + charHeight < clipRect.y1 )
		return;

	register char  * image             = (char *) curFont -> charImage ( ch );
	register short * addr              = (short *) pixelAddr ( x, y );
	int              imageBytesPerLine = (charWidth+7) >> 3;
	int              lx                = x < clipRect.x1 ? clipRect.x1 - x : 0;

	if ( x + charWidth > clipRect.x2 )
		charWidth = clipRect.x2 - x;

	for ( register int i = 0; i < charHeight; i++, addr += bytesPerScanLine >> 1, image += imageBytesPerLine )
	{
		if ( y + i < clipRect.y1 )
			continue;

		if ( y + i > clipRect.y2 )
			break;

		if ( rasterOp == RO_COPY )
		{
			for ( register int j = lx; j < charWidth; j++ )
				if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
					addr [j] = (short) color;
		}
		else
		if ( rasterOp == RO_OR )
		{
			for ( register int j = lx; j < charWidth; j++ )
				if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
					addr [j] |= (short) color;
		}
		else
		if ( rasterOp == RO_AND )
		{
			for ( register int j = lx; j < charWidth; j++ )
				if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
					addr [j] &= (short) color;
		}
		else
		if ( rasterOp == RO_XOR )
		{
			for ( register int j = lx; j < charWidth; j++ )
				if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
					addr [j] ^= (short) color;
		}
	}
}

void    Surface :: drawString8 ( int x,  int y, const char * str )
{
	x += org.x;
	y += org.y;

        if ( curFont == NULL || x > clipRect.x2 || y > clipRect.y2 )
                return;

        int     strHeight = curFont -> height;

        if ( y + strHeight < clipRect.y1 )
                return;

        int     lx, rx;
        int     charWidth;
        char  * saveAddr;

        if ( x < clipRect.x1 )
        {
                charWidth = curFont -> charWidth ( *str );

                while ( x + charWidth < clipRect.x1 )
                {
			if ( *++str == '\0' )
                                return;

                        x        += charWidth;
                        charWidth = curFont -> charWidth ( *str );
                }

                lx = clipRect.x1 - x;
        }
        else
                lx = 0;

        while ( * str != '\0' && x < clipRect.x2 )
        {
                char  * image    = (char *) curFont -> charImage ( *str );
                char  * addr     = (char *) pixelAddr ( x, y );

                saveAddr  = addr;
                charWidth = curFont -> charWidth ( *str++ );
                rx        = x + charWidth < clipRect.x2 ? charWidth : clipRect.x2 - x;

                for ( register int i = 0; i < strHeight; i++, addr  += bytesPerScanLine, image += (charWidth+7)>>3 )
                {
                        if ( y + i < clipRect.y1 )
                                continue;

                        if ( y + i > clipRect.y2 )
                                break;

                        if ( rasterOp == RO_COPY )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] = (char) color;
                        }
                        else
                        if ( rasterOp == RO_OR )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
						addr [j] |= (char) color;
                        }
                        else
                        if ( rasterOp == RO_AND )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] &= (char) color;
                        }
                        else
                        if ( rasterOp == RO_AND )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] ^= (char) color;
                        }
                }

                addr = saveAddr + charWidth;
                x   += charWidth;
		lx   = 0;
	}
}

void    Surface :: drawString16 ( int x,  int y, const char * str )
{
	x += org.x;
	y += org.y;

	if ( curFont == NULL || x > clipRect.x2 || y > clipRect.y2 )
		return;

	int     strHeight = curFont -> height;

	if ( y + strHeight < clipRect.y1 )
		return;

	int     lx, rx;
	int     charWidth;
	short * saveAddr;

	if ( x < clipRect.x1 )
	{
		charWidth = curFont -> charWidth ( *str );

		while ( x + charWidth < clipRect.x1 )
		{
			if ( *++str == '\0' )
				return;

			x        += charWidth;
			charWidth = curFont -> charWidth ( *str );
		}

		lx = clipRect.x1 - x;
	}
	else
		lx = 0;

	while ( * str != '\0' && x < clipRect.x2 )
	{
		char  * image    = (char *) curFont -> charImage ( *str );
		short * addr     = (short *) pixelAddr ( x, y );

		saveAddr  = addr;
		charWidth = curFont -> charWidth ( *str++ );
		rx        = x + charWidth < clipRect.x2 ? charWidth : clipRect.x2 - x;

		for ( register int i = 0; i < strHeight; i++, addr += bytesPerScanLine >> 1, image += (charWidth+7)>>3 )
		{
			if ( y + i < clipRect.y1 )
				continue;

                        if ( y + i > clipRect.y2 )
                                break;

                        if ( rasterOp == RO_COPY )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] = (short) color;
                        }
                        else
                        if ( rasterOp == RO_OR )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] |= (short) color;
			}
                        else
                        if ( rasterOp == RO_AND )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] &= (short) color;
                        }
                        else
                        if ( rasterOp == RO_AND )
                        {
                                for ( register int j = lx; j <= rx; j++ )
                                        if ( image [j >> 3] & (0x80 >> (j & 7)) )
                                                addr [j] ^= (short) color;
                        }
                }

		addr = saveAddr + charWidth;
		x   += charWidth;
		lx   = 0;
	}
}

void    Surface :: drawBar8    ( int x1, int y1, int x2, int y2 )
{
        Rect    r ( x1 + org.x, y1 + org.y, x2 + org.x, y2 + org.y );

        r &= clipRect;                  // clip to current rect

        if ( r.isEmpty () )
                return;

        register char * addr = (char *) pixelAddr ( 0, r.y1 );

        if ( rasterOp == RO_COPY )
                for ( register int y = r.y1; y <= r.y2; y++ )
                {
                        for ( register int x = r.x1; x <= r.x2; x++ )
				addr [x] = (char) color;

                        addr += bytesPerScanLine;
                }
        else
        if ( rasterOp == RO_OR )
                for ( register int y = r.y1; y <= r.y2; y++ )
                {
                        for ( register int x = r.x1; x <= r.x2; x++ )
                                addr [x] |= (char) color;

                        addr += bytesPerScanLine;
                }
        else
        if ( rasterOp == RO_AND )
                for ( register int y = r.y1; y <= r.y2; y++ )
                {
                        for ( register int x = r.x1; x <= r.x2; x++ )
                                addr [x] &= (char) color;

			addr += bytesPerScanLine;
                }
        else
        if ( rasterOp == RO_XOR )
                for ( register int y = r.y1; y <= r.y2; y++ )
                {
                        for ( register int x = r.x1; x <= r.x2; x++ )
                                addr [x] ^= (char) color;

                        addr += bytesPerScanLine;
                }
}

void    Surface :: drawBar16    ( int x1, int y1, int x2, int y2 )
{
        Rect    r ( x1 + org.x, y1 + org.y, x2 + org.x, y2 + org.y );

        r &= clipRect;                  // clip to current rect

        if ( r.isEmpty () )
		return;

	register short * addr = (short *) pixelAddr ( 0, r.y1 );

	if ( rasterOp == RO_COPY )
		for ( register int y = r.y1; y <= r.y2; y++ )
		{
			for ( register int x = r.x1; x <= r.x2; x++ )
				addr [x] = (short) color;

			addr += bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_OR )
		for ( register int y = r.y1; y <= r.y2; y++ )
		{
			for ( register int x = r.x1; x <= r.x2; x++ )
				addr [x] |= (short) color;

			addr += bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_AND )
		for ( register int y = r.y1; y <= r.y2; y++ )
		{
			for ( register int x = r.x1; x <= r.x2; x++ )
				addr [x] &= (short) color;

			addr += bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_XOR )
		for ( register int y = r.y1; y <= r.y2; y++ )
		{
			for ( register int x = r.x1; x <= r.x2; x++ )
				addr [x] ^= (short) color;

			addr += bytesPerScanLine >> 1;
		}
}

void    Surface :: copy8 ( Surface& dstSurface, const Rect& srcRect, const Point& dstPoint )
{
	Rect    src ( srcRect );
	Point   dst ( dstPoint );
	int     delta;

	dst += dstSurface.org;

	if ( ( delta = dstSurface.clipRect.x1 - dst.x ) > 0 )
	{
		src.x1 += delta;
		dst.x  += delta;
	}

	if ( ( delta = dstSurface.clipRect.y1 - dst.y ) > 0 )
	{
		src.y1 += delta;
		dst.y  += delta;
	}

	if ( ( delta = dst.x + srcRect.width () - 1 - dstSurface.clipRect.x2 ) > 0 )
		src.x2 -= delta;

	if ( ( delta = dst.y + srcRect.height () - 1 - dstSurface.clipRect.y2 ) > 0 )
		src.y2 -= delta;

	if ( src.isEmpty () )
		return;

	char  * srcAddr   = (char *) pixelAddr ( src.x1, src.y1 );
	char  * dstAddr   = (char *) dstSurface.pixelAddr ( dst.x, dst.y );
        int     lineBytes = src.width () * format.bytesPerPixel;

        if ( rasterOp == RO_COPY )
                for ( int y = src.y1; y <= src.y2; y++ )
                {
                        memcpy ( dstAddr, srcAddr, lineBytes );

                        srcAddr += bytesPerScanLine;
                        dstAddr += dstSurface.bytesPerScanLine;
                }
        else
        if ( rasterOp == RO_OR )
                for ( int y = src.y1; y <= src.y2; y++ )
                {
                        for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
                                dstAddr [i] |= srcAddr [i];

                        srcAddr += bytesPerScanLine;
                        dstAddr += dstSurface.bytesPerScanLine;
		}
        else
        if ( rasterOp == RO_AND )
                for ( int y = src.y1; y <= src.y2; y++ )
                {
                        for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
                                dstAddr [i] &= srcAddr [i];

                        srcAddr += bytesPerScanLine;
                        dstAddr += dstSurface.bytesPerScanLine;
                }
        else
        if ( rasterOp == RO_XOR )
                for ( int y = src.y1; y <= src.y2; y++ )
                {
                        for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
                                dstAddr [i] ^= srcAddr [i];

                        srcAddr += bytesPerScanLine;
                        dstAddr += dstSurface.bytesPerScanLine;
		}
}

void    Surface :: copy16 ( Surface& dstSurface, const Rect& srcRect, const Point& dstPoint )
{
        Rect    src ( srcRect );
        Point   dst ( dstPoint );
        int     delta;

        dst += dstSurface.org;

        if ( ( delta = dstSurface.clipRect.x1 - dst.x ) > 0 )
        {
                src.x1 += delta;
                dst.x  += delta;
        }

        if ( ( delta = dstSurface.clipRect.y1 - dst.y ) > 0 )
        {
                src.y1 += delta;
		dst.y  += delta;
	}

	if ( ( delta = dst.x + srcRect.width () - 1 - dstSurface.clipRect.x2 ) > 0 )
		src.x2 -= delta;

	if ( ( delta = dst.y + srcRect.height () - 1 - dstSurface.clipRect.y2 ) > 0 )
		src.y2 -= delta;

	if ( src.isEmpty () )
		return;

	short * srcAddr   = (short *) pixelAddr ( src.x1, src.y1 );
	short * dstAddr   = (short *) dstSurface.pixelAddr ( dst.x, dst.y );
	int     lineBytes = src.width () * format.bytesPerPixel;

	if ( rasterOp == RO_COPY )
		for ( int y = src.y1; y <= src.y2; y++ )
		{
			memcpy ( dstAddr, srcAddr, lineBytes );

			srcAddr += bytesPerScanLine >> 1;
			dstAddr += dstSurface.bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_OR )
		for ( int y = src.y1; y <= src.y2; y++ )
		{
			for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
				dstAddr [i] |= srcAddr [i];

			srcAddr += bytesPerScanLine >> 1;
			dstAddr += dstSurface.bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_AND )
		for ( int y = src.y1; y <= src.y2; y++ )
		{
			for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
				dstAddr [i] &= srcAddr [i];

			srcAddr += bytesPerScanLine >> 1;
			dstAddr += dstSurface.bytesPerScanLine >> 1;
		}
	else
	if ( rasterOp == RO_XOR )
		for ( int y = src.y1; y <= src.y2; y++ )
		{
			for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
				dstAddr [i] ^= srcAddr [i];

			srcAddr += bytesPerScanLine >> 1;
			dstAddr += dstSurface.bytesPerScanLine >> 1;
		}
}

void    Surface :: copyTransp8 ( Surface& dstSurface, const Rect& srcRect, const Point& dstPoint, int transpColor )
{
	Rect    src ( srcRect );
	Point   dst ( dstPoint );
	int     delta;

	dst += dstSurface.org;

        if ( ( delta = dstSurface.clipRect.x1 - dst.x ) > 0 )
        {
                src.x1 += delta;
                dst.x  += delta;
        }

        if ( ( delta = dstSurface.clipRect.y1 - dst.y ) > 0 )
        {
                src.y1 += delta;
                dst.y  += delta;
        }

        if ( ( delta = dst.x + srcRect.width () - 1 - dstSurface.clipRect.x2 ) > 0 )
                src.x2 -= delta;

        if ( ( delta = dst.y + srcRect.height () - 1 - dstSurface.clipRect.y2 ) > 0 )
                src.y2 -= delta;

        if ( src.isEmpty () )
		return;

        register char * srcAddr   = (char *) pixelAddr ( srcRect.x1, srcRect.y1 );
        register char * dstAddr   = (char *) dstSurface.pixelAddr ( dst.x, dst.y );

        for ( int y = src.y1; y <= src.y2; y++ )
        {
                for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
                        if ( srcAddr [i] != transpColor )
                                dstAddr [i] = srcAddr [i];

                srcAddr += bytesPerScanLine;
                dstAddr += dstSurface.bytesPerScanLine;
        }
}

void    Surface :: copyTransp16 ( Surface& dstSurface, const Rect& srcRect, const Point& dstPoint, int transpColor )
{
        Rect    src ( srcRect );
        Point   dst ( dstPoint );
	int     delta;

	dst += dstSurface.org;

	if ( ( delta = dstSurface.clipRect.x1 - dst.x ) > 0 )
	{
		src.x1 += delta;
		dst.x  += delta;
	}

	if ( ( delta = dstSurface.clipRect.y1 - dst.y ) > 0 )
	{
		src.y1 += delta;
		dst.y  += delta;
	}

	if ( ( delta = dst.x + srcRect.width () - 1 - dstSurface.clipRect.x2 ) > 0 )
		src.x2 -= delta;

	if ( ( delta = dst.y + srcRect.height () - 1 - dstSurface.clipRect.y2 ) > 0 )
		src.y2 -= delta;

	if ( src.isEmpty () )
		return;

	register short * srcAddr   = (short *) pixelAddr ( srcRect.x1, srcRect.y1 );
	register short * dstAddr   = (short *) dstSurface.pixelAddr ( dst.x, dst.y );

	for ( int y = src.y1; y <= src.y2; y++ )
	{
		for ( register int x = src.x1, i = 0; x <= src.x2; x++, i++ )
			if ( srcAddr [i] != transpColor )
				dstAddr [i] = srcAddr [i];

		srcAddr += bytesPerScanLine >> 1;
		dstAddr += dstSurface.bytesPerScanLine >> 1;
	}
}

void    Surface :: scroll ( const Rect& srcRect, const Point& dstPoint )
{
	Rect    src ( srcRect );
	Point   dst ( dstPoint );
	int     delta;

	dst += org;

	if ( ( delta = clipRect.x1 - dst.x ) > 0 )
	{
		src.x1 += delta;
		dst.x  += delta;
	}

	if ( ( delta = clipRect.y1 - dst.y ) > 0 )
	{
		src.y1 += delta;
		dst.y  += delta;
	}

	if ( ( delta = dst.x + srcRect.width () - 1 - clipRect.x2 ) > 0 )
		src.x2 -= delta;

	if ( ( delta = dst.y + srcRect.height () - 1 - clipRect.y2 ) > 0 )
		src.y2 -= delta;

	if ( src.isEmpty () )
		return;

	if ( dst.y > src.y1 )   // may overlap, copy in reverse order
	{
		char  * srcAddr   = (char *) pixelAddr ( src.x1, src.y2 );
		char  * dstAddr   = (char *) pixelAddr ( dst.x, dst.y + src.height () - 1 );
		int     lineBytes = src.width () * format.bytesPerPixel;

		for ( int y = src.y2; y >= src.y1; y-- )
		{
			memmove ( dstAddr, srcAddr, lineBytes );

			srcAddr -= bytesPerScanLine;
			dstAddr -= bytesPerScanLine;
		}
	}
	else                    // copy in normal order
        {
                char  * srcAddr   = (char *) pixelAddr ( src.x1, src.y1 );
                char  * dstAddr   = (char *) pixelAddr ( dst.x, dst.y );
                int     lineBytes = src.width () * format.bytesPerPixel;

                for ( int y = src.y1; y <= src.y2; y++ )
                {
                        memmove ( dstAddr, srcAddr, lineBytes );

                        srcAddr += bytesPerScanLine;
                        dstAddr += bytesPerScanLine;
                }
        }
}

Object * loadSurface ()
{
        return NULL;
}
