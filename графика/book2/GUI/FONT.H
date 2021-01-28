//
// Abstract font class
// Author : Alex V. Boreskoff
//
#ifndef __FONT__
#define __FONT__

#include        <malloc.h>
#include        "object.h"

struct  CharInfo
{
        short   width;                          // width of character in pixels
        short   start;                          // offset of character data in font buffer
};

class   Font : public Object
{
public:
        void      * data;                       // binary images of chars
        int         width;                      // width (in pixels) for fixed pith or 0
        int         height;                     // height of font  (in pixels)
        int         baseLine;
        char        firstChar;
        char        lastChar;
        long        imageSize;                  // size of images of all chars (in bytes)
        CharInfo  * charData;                   // pointers to character data (width, image)

	Font ()
        {
                data     = NULL;
                charData = NULL;
        }
        ~Font ()
        {
                free ( data );

                delete [] charData;
        }

        virtual int     isOk () const
        {
                return data != NULL && charData != NULL;
        }

	void    size ( int& w, int& h, const char * str) const;

        int     isFixed () const
        {
                return width != 0;
        }

	virtual	int     charWidth ( int ch )
	{
		return charData [ch-firstChar].width;
	}

	virtual	void  * charImage ( int ch )
	{
		return charData [ch-firstChar].start + (char *) data;
	}
};

#endif
