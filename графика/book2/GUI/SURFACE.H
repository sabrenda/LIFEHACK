//
// Basic class for images and all 2D graphics
// You can add your own functions
// currently supports only 8-bit and 15/16-bit modes
//
// Author: Alex V. Boreskoff
// Last change: 01/02/1999
//
#ifndef __SURFACE__
#define __SURFACE__

#include        <malloc.h>
#include        "rect.h"
#include        "object.h"
#include        "font.h"

enum    RasterOperations
{
        RO_COPY,
        RO_OR,
        RO_AND,
        RO_XOR
};

class  RGB
{
public:
	char    red;
	char    green;
	char    blue;

	RGB () {}
	RGB ( int r, int g, int b )
	{
		red   = r;
		green = g;
		blue  = b;
	}
};

struct  PixelFormat
{
	unsigned redMask;                // bit mask for red color bits
	int      redShift;               // position of red bits in pixel data
	int      redBits;                // # of bits for red field
	unsigned greenMask;              // bit mask for green color bits
	int      greenShift;             // position of green bits in pixel data
	int      greenBits;              // # of bits for green field
	unsigned blueMask;               // bit mask for blue color bits
	int      blueShift;              // position of blue bits in pixel data
	int      blueBits;
	int      bitsPerPixel;           // # of bits per pixel
	int      bytesPerPixel;          // # of bytes per single pixel

	void    completeFromMasks ();
};

inline int      operator == ( const PixelFormat& f1, const PixelFormat& f2 )
{
        return f1.redMask == f2.redMask && f1.greenMask == f2.greenMask && f1.blueMask == f2.blueMask;
}

inline  int     rgbTo24Bit ( int red, int green, int blue )
{
        return blue | (green << 8) | (red << 16);
}

inline  int     rgbTo16Bit ( int red, int green, int blue )
{
        return (blue>>3) | ((green>>2)<<5) | ((red>>3)<<10);
}

inline  int     rgbTo15Bit ( int red, int green, int blue )
{
        return (blue>>3) | ((green>>3)<<5) | ((red>>3)<<10);
}

inline  int     rgbToInt ( int red, int green, int blue, const PixelFormat& fmt )
{
        return ((blue>>(8-fmt.blueBits))<<fmt.blueShift) | ((green>>(8-fmt.greenBits))<<fmt.greenShift) |
               ((red>>(8-fmt.redBits))<<fmt.redShift);
}

class   Surface : public Object
{
protected:                              // function pointers
	int     (Surface :: *getPixelAddr)   ( int x,  int y );
	void    (Surface :: *drawPixelAddr)  ( int x,  int y, int color );
	void    (Surface :: *drawLineAddr)   ( int x1, int y1, int x2, int y2 );
	void    (Surface :: *drawCharAddr)   ( int x,  int y, int ch );
	void    (Surface :: *drawStringAddr) ( int x,  int y, const char * str );
	void    (Surface :: *drawBarAddr)    ( int x1, int y1, int x2, int y2 );
	void    (Surface :: *copyAddr)       ( Surface& dstSurface, const Rect& srcRect, const Point& dst );
	void    (Surface :: *copyTranspAddr) ( Surface& dstSurface, const Rect& srcRect, const Point& dst, int transpColor );

public:
	int         bytesPerScanLine;   // # of bytes per single scan line
	PixelFormat format;
	int         width;
	int         height;
	void      * data;               // image data
	RGB       * palette;            // currently used palette (or NULL)
	Rect        clipRect;           // area to clip
	Font      * curFont;
	int         color;              // current draw color
	int         backColor;          // current background color
	int         rasterOp;           // current raster operation
	Point       org;                // drawing offset
					// 8-bit (256 colors mode) functions
	int     getPixel8    ( int x,  int y );
	void    drawPixel8   ( int x,  int y, int color );
	void    drawLine8    ( int x1, int y1, int x2, int y2 );
	void    drawChar8    ( int x,  int y, int ch );
	void    drawString8  ( int x,  int y, const char * str );
	void    drawBar8     ( int x1, int y1, int x2, int y2 );
	void    copy8        ( Surface& dstSurface, const Rect& srcRect, const Point& dst );
	void    copyTransp8  ( Surface& dstSurface, const Rect& srcRect, const Point& dst, int transpColor );
					// 15/16bit functions
	int     getPixel16    ( int x,  int y );
	void    drawPixel16  ( int x,  int y, int color );
	void    drawLine16   ( int x1, int y1, int x2, int y2 );
	void    drawChar16   ( int x,  int y, int ch );
	void    drawString16 ( int x,  int y, const char * str );
	void    drawBar16    ( int x1, int y1, int x2, int y2 );
	void    copy16       ( Surface& dstSurface, const Rect& srcRect, const Point& dst );
	void    copyTransp16 ( Surface& dstSurface, const Rect& srcRect, const Point& dst, int transpColor );

public:
	Surface () : Object () {}
	Surface ( int w, int h, const PixelFormat& fmt );

	virtual ~Surface ()
	{
		if ( data != NULL )
			free ( data );

		if ( palette != NULL )
			delete palette;
	}

	virtual char  * getClassName () const
	{
		return "Surface";
	}

	virtual int     isOk () const
	{
		return data != NULL && width > 0 && height > 0;
	}

	virtual	int	put ( Store * ) const;
	virtual	int	get ( Store * );

	void	setFuncPointers ();

	int	closestColor ( const RGB& color ) const;

	virtual void    beginDraw () {}         // lock memory
	virtual void    endDraw () {}           // unlock memory

	void  * pixelAddr  ( const int x, const int y ) const
	{
		return x * format.bytesPerPixel + y * bytesPerScanLine + (char *) data;
	}

	void    setClipRect ( const Rect& r )
	{
		clipRect = r;
	}

        void    setOrg ( const Point& p )
        {
                org = p;
        }

        void    setFont ( Font * fnt )
	{
                curFont = fnt;
        }

	void    setColor ( int fg )
	{
		color = fg;
	}

	void	setBkColor ( int bkColor )
	{
		backColor = bkColor;
	}

	void    setRop ( int rop = RO_COPY )
        {
                rasterOp = rop;
        }

	int     getRop ()
        {
                return rasterOp;
	}

        Rect    getClipRect ()
        {
                return clipRect;
        }

        Point   getOrg ()
        {
                return org;
        }

	void    drawFrame      ( const Rect& r );
	void    scroll         ( const Rect& area, const Point& dst );

        int     getPixel ( int x, int y )
        {
                return (this->*getPixelAddr)( x, y );
        }

        void    drawPixel ( int x,  int y, int color )
        {
                (this->*drawPixelAddr)( x, y, color );
        }

        void    drawLine ( int x1, int y1, int x2, int y2 )
        {
                (this->*drawLineAddr)( x1, y1, x2, y2 );
        }

        void    drawChar ( int x,  int y, int ch )
	{
                (this->*drawCharAddr)( x, y, ch );
        }

        void    drawString ( int x,  int y, const char * str )
        {
                (this->*drawStringAddr)( x, y, str );
	}

        void    drawBar ( int x1, int y1, int x2, int y2 )
        {
                (this->*drawBarAddr)( x1, y1, x2, y2 );
        }

        void    copy ( Surface& dstSurface, const Rect& srcRect, const Point& dst )
        {
                (this->*copyAddr)( dstSurface, srcRect, dst );
        }

	void    copyTransp ( Surface& dstSurface, const Rect& srcRect, const Point& dst, int transpColor )
        {
		(this->*copyTranspAddr)( dstSurface, srcRect, dst, transpColor );
	}

	void    copy ( Surface& dstSurface, const Point& dstPoint )
	{
		(this->*copyAddr) ( dstSurface, Rect ( 0, 0, width - 1, height - 1 ), dstPoint );
	}

	void    copyTransp  ( Surface& dstSurface, const Point& dstPoint, int transpColor )
	{
		(this->*copyTranspAddr) ( dstSurface, Rect ( 0, 0, width - 1, height - 1 ), dstPoint, transpColor );
	}
};

Object * loadSurface ();

#endif
