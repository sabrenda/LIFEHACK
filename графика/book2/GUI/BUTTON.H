//
// Button classes for GUI
// Author : Alex V. Boreskoff
//
#ifndef	__BUTTON__
#define	__BUTTON__

#include "view.h"
#include "surface.h"

			// text styles
#define	TS_RALIGN		0x0008
#define	TS_CENTER		0x0010

			// button status
#define	BS_PRESSED		0x0008
#define	BS_CHECKED		0x0010
#define	BS_TRACKING		0x0020

///////////////////////////////////////////////////////////////////////////

class	PushButton : public View
{
public:
	PushButton ( int x, int y, int w, int h, View * p, int anId ) : View ( x, y, w, h, p )
	{
		tag     = anId;
		status |= WS_VISIBLE | WS_ENABLED;
		style  |= WS_REPAINTONFOCUS;
	}

	PushButton () : View () {}

	virtual	char  * getClassName () const
	{
		return "PushButton";
	}

	void		performClick ();

	virtual	int	keyDown   ( const Message& );
	virtual	int	mouseDown ( const Message& );
	virtual	int	mouseUp   ( const Message& );
	virtual	int	mouseMove ( const Message& );
};

class	TextButton : public PushButton
{
public:
	TextButton ( int x, int y, int w, int h, View * p, int anId, char * t ) : PushButton ( x, y, w, h, p, anId )
	{
		strcpy ( text, t );
	}

	TextButton () : PushButton () {}

	virtual	char  * getClassName () const
	{
		return "TextButton";
	}

	virtual	void	draw ( const Rect& ) const;
};

class	PicButton : public PushButton
{
public:
	Surface * face [3];		// normal, pressed, focused

	PicButton ( int, int, View *, Surface *, Surface *, Surface *, int );
	PicButton () : PushButton () {}
	~PicButton ();

	virtual	char  * getClassName () const
	{
		return "PicButton";
	}

	virtual	int	put ( Store * ) const;
	virtual	int	get ( Store * );

	virtual	void	draw ( const Rect& ) const;
};

#endif