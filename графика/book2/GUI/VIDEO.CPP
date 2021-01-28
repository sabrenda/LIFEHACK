//
// Basic video functions & constants
// Author: Alex V. Boreskoff
//
#include        "video.h"
#include        "surface.h"
#include        "store.h"
#include        "loader.h"
#include        "vesasurf.h"
#include        "rasfont.h"
#include        "memory.h"

ResFile * systemResource = NULL;
Surface * screenSurface  = NULL;
int     * palette        = NULL;
Rect      screenRect;

static int     pal8bit [16] =
{
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15
};

static int     directColorPal [16];

static int     pal15bit [16] =          // 15-bit palette
{
        rgbTo15Bit ( 0, 0, 0 ),         // black
        rgbTo15Bit ( 0, 0, 0x80 ),      // blue
        rgbTo15Bit ( 0, 0x80, 0 ),      // green
        rgbTo15Bit ( 0, 0x80, 0x80 ),   // cyan
        rgbTo15Bit ( 0x80, 0, 0 ),      // red
        rgbTo15Bit ( 0x80, 0, 0x80 ),   // magenta
        rgbTo15Bit ( 0x80, 0x80, 0 ),   // brown
        rgbTo15Bit ( 0x80, 0x80, 0x80 ),// darkgray
        rgbTo15Bit ( 0xC0, 0xC0, 0xC0 ),// lightgray
        rgbTo15Bit ( 0, 0, 0xFF ),      // light blue
        rgbTo15Bit ( 0, 0xFF, 0 ),      // light green
        rgbTo15Bit ( 0, 0xFF, 0xFF ),   // light cyan
        rgbTo15Bit ( 0xFF, 0, 0 ),      // light red
        rgbTo15Bit ( 0xFF, 0, 0xFF ),   // light magenta
        rgbTo15Bit ( 0xFF, 0xFF, 0 ),   // yellow
        rgbTo15Bit ( 0xFF, 0xFF, 0xFF ) // white
};

static int     pal16bit [16] =          // 16-bit palette
{
        rgbTo16Bit ( 0, 0, 0 ),         // black
        rgbTo16Bit ( 0, 0, 0x80 ),      // blue
        rgbTo16Bit ( 0, 0x80, 0 ),      // green
        rgbTo16Bit ( 0, 0x80, 0x80 ),   // cyan
        rgbTo16Bit ( 0x80, 0, 0 ),      // red
        rgbTo16Bit ( 0x80, 0, 0x80 ),   // magenta
        rgbTo16Bit ( 0x80, 0x80, 0 ),   // brown
        rgbTo16Bit ( 0x80, 0x80, 0x80 ),// darkgray
        rgbTo16Bit ( 0xC0, 0xC0, 0xC0 ),// lightgray
        rgbTo16Bit ( 0, 0, 0xFF ),      // light blue
        rgbTo16Bit ( 0, 0xFF, 0 ),      // light green
        rgbTo16Bit ( 0, 0xFF, 0xFF ),   // light cyan
        rgbTo16Bit ( 0xFF, 0, 0 ),      // light red
        rgbTo16Bit ( 0xFF, 0, 0xFF ),   // light magenta
        rgbTo16Bit ( 0xFF, 0xFF, 0 ),   // yellow
        rgbTo16Bit ( 0xFF, 0xFF, 0xFF ) // white
};

Surface * loadBMPFromResource ( ResFile * resource, const char * resName )
{
        int     size;
        void  * data = NULL;
        int     code;

        size = resource -> get ( resName, data );

        if ( size < 1 || data == NULL )
                return NULL;

        Surface * res = loadBMP ( data, size, *screenSurface, code );

        free ( data );

        return res;
}

Font * loadFontFromResource ( ResFile * resource, const char * resName )
{
        int     size;
        void  * data = NULL;
        int     code;

        size = resource -> get ( resName, data );

        if ( size < 1 || data == NULL )
                return NULL;

        MemoryStore    file ( data, size );

        return new RasterFont ( &file );
}

int     initVideo ( int xRes, int yRes, int bpp )
{
        systemResource = new ResFile ( "gui.res" );

        if ( !systemResource -> isOk () )
                return FALSE;

        screenSurface = new VESASurface ( xRes, yRes, bpp );

        if ( !screenSurface -> isOk () )
                return FALSE;

        screenRect = Rect ( 0, 0, screenSurface -> width - 1, screenSurface -> height - 1 );

        if ( screenSurface -> format.bytesPerPixel == 1 )
                palette = pal8bit;
        else
        {
                directColorPal [0]  = rgbToInt ( 0, 0, 0, screenSurface -> format );         // black
                directColorPal [1]  = rgbToInt ( 0, 0, 0x80, screenSurface -> format );      // blue
                directColorPal [2]  = rgbToInt ( 0, 0x80, 0, screenSurface -> format );      // green
                directColorPal [3]  = rgbToInt ( 0, 0x80, 0x80, screenSurface -> format );   // cyan
                directColorPal [4]  = rgbToInt ( 0x80, 0, 0, screenSurface -> format );      // red
                directColorPal [5]  = rgbToInt ( 0x80, 0, 0x80, screenSurface -> format );   // magenta
                directColorPal [6]  = rgbToInt ( 0x80, 0x80, 0, screenSurface -> format );   // brown
		directColorPal [7]  = rgbToInt ( 0xC0, 0xC0, 0xC0, screenSurface -> format );// lightgray
		directColorPal [8]  = rgbToInt ( 0x80, 0x80, 0x80, screenSurface -> format );// darkgray
		directColorPal [9]  = rgbToInt ( 0, 0, 0xFF, screenSurface -> format );      // light blue
		directColorPal [10] = rgbToInt ( 0, 0xFF, 0, screenSurface -> format );      // light green
		directColorPal [11] = rgbToInt ( 0, 0xFF, 0xFF, screenSurface -> format );   // light cyan
		directColorPal [12] = rgbToInt ( 0xFF, 0, 0, screenSurface -> format );      // light red
		directColorPal [13] = rgbToInt ( 0xFF, 0, 0xFF, screenSurface -> format );   // light magenta
		directColorPal [14] = rgbToInt ( 0xFF, 0xFF, 0, screenSurface -> format );   // yellow
		directColorPal [15] = rgbToInt ( 0xFF, 0xFF, 0xFF, screenSurface -> format );// white

		palette = directColorPal;
        }

        return TRUE;
}

int     doneVideo ()
{
        delete screenSurface;
        delete systemResource;

        return TRUE;
}
