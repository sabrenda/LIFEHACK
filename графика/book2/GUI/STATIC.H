//
// Static text class for GUI
// Author: Alex V. Boreskoff
//
#ifndef	__STATIC__
#define	__STATIC__

#include "win.h"
#include "font.h"
			// text styles
#define	TS_RALIGN		0x0008
#define	TS_CENTER		0x0010

			// static styles
#define	SS_GRAY			'\x00'
#define	SS_BLACK		'\x01'
#define	SS_3D			'\x02'
#define	SS_DOWN			'\x03'
#define	SS_UP			'\x04'
#define	SS_FRAMED		'\x05'
#define	SS_FRAMERAISED		'\x06'

///////////////////////////////////////////////////////////////////////////

class	StaticText : public View
{
public:
	Font	* fnt;			// font used to draw text
	int	  backColor, textColor;	// colors for background & text
	int	  borderType;
	int	  shouldDelete;

	StaticText ( int x, int y, int w, int h, View * p, char * t, int tc, int bc, char br = SS_3D, Font * fn = sysFont, int del = FALSE );
	StaticText () : View () {}
	~StaticText ()
	{
		if ( shouldDelete )
			delete fnt;
	}

	virtual	char  * getClassName () const
	{
		return "StaticText";
	}

	virtual	int	put ( Store * ) const;
	virtual	int	get ( Store * );

	virtual	int     mouseDown ( const Message& ) const
	{
		return TRUE;
	}

	virtual	int	canReceiveFocus () const
	{
		return FALSE;
	}

	virtual	void	draw ( const Rect& ) const;
};

class	StaticPic : public View
{
public:
	Surface * pic;

	StaticPic ( int x, int y, View * p, Surface * bm ) : View ( x, y, bm -> width, bm -> height, p )
	{
		pic     = bm;
		status |= WS_VISIBLE;
	}

	StaticPic () : View () {}

	~StaticPic ()
	{
		delete pic;
	}

	virtual	char  * getClassName () const
	{
		return "StaticPic";
	}

	virtual	int     mouseDown ( const Message& ) const
	{
		return TRUE;
	}

	virtual	int	canReceiveFocus () const
	{
		return FALSE;
	}

	virtual	void	draw ( const Rect& r ) const
	{
		pic -> copy ( *screenSurface, Point ( 0, 0 ) );
	}
};

Object * staticTextLoader ();
Object * staticPicLoader ();

#endif