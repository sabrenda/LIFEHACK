//
// Standart window & dialog window classes, desktop class
// Author: Alex V. Boreskoff
//
#ifndef	__WIN__
#define	__WIN__

#include	"view.h"
#include	"font.h"
#include	"surface.h"

				// status bits
#define	WS_ACTIVE		0x00000008

				// style bits
#define	WS_MOVEABLE		0x00000008
#define	WS_RESIZEABLE		0x00000010
#define	WS_HASCLOSEBOX		0x00000020
#define	WS_HASICONBOX		0x00000040

				// hit codes
#define	HT_CAPTION	1
#define	HT_CLOSE	2
#define	HT_RESIZE	3
#define	HT_RESIZEVER	4

#define	CAPTION_SIZE	21

///////////////////////////////////////////////////////////////////////////

class	DeskTop : public View
{
public:
	int	running;

	DeskTop ();

	virtual char * getClassName () const    // return object's class name
	{
		return "DeskTop";
	}

	virtual	int	handle  ( const Message& );
	virtual	int	command ( const Message&  );
	virtual	void	draw    ( const Rect& ) const;
	virtual	void	run     ();
	virtual	void	idle    () {}
};

///////////////////////////////////////////////////////////////////////////

class	Window : public View
{
public:
	Window ( int x, int y, int w, int h, char * t, View * p ) : View ( x, y, w, h, p )
	{
		strcpy ( text, t );
		style  |= WS_MOVEABLE | WS_RESIZEABLE | WS_ACTIVATEABLE | WS_HASCLOSEBOX;
		status |= WS_ENABLED;
	};

	Window () : View () {}

	virtual char * getClassName () const    // return object's class name
	{
		return "Window";
	}

	virtual	int	receiveFocus ( const Message& );
	virtual	int	command ( const Message& );
	virtual	int	mouseDown ( const Message& );
	virtual	void	draw ( const Rect& ) const;

	virtual	Rect	getClientRect ( Rect& ) const;
	virtual	void	getMinMaxSize ( Point& minSize, Point& maxSize ) const;
	virtual	void	drawFrame  () const;
	virtual	int	hitTest    ( const Point& ) const;
	virtual	void	activate   ();
	virtual	void	deactivate ();

	int		isActive () const
	{
		return status & WS_ACTIVE;
	}

private:
	void	trackClose  ();
	void	trackDrag   ( Point );
	void	trackResize ( int, Point );
};

///////////////////////////////////////////////////////////////////////////

class	DialogWindow : public Window
{
public:
	int	dialogCode;
	int	dialogActive;
	int	defaultID;
	View *	saveFocus;

	DialogWindow ( int, int, int, int, char *, View * );
	DialogWindow () : Window () {}

	virtual char * getClassName () const    // return object's class name
	{
		return "DialogWindow";
	}
						// whether the object confroms to given class
	virtual int      put ( Store * ) const; // save object to the store
	virtual int      get ( Store * );       // load object from the store

	void		setDefID ( int id )
	{
		defaultID = id;
	}
	void		endDialog ( int code )
	{
		dialogCode   = code;
		dialogActive = FALSE;
	}

	int		execute ();
	virtual	void	initDialog () {}
	virtual	void	prepDialog () {}
	virtual	void	idle () {}

	virtual	int     handle ( const Message&  );
	virtual	int	keyDown ( const Message& );
	virtual	int	receiveFocus ( const Message& );
	virtual	int	command ( const Message& );
	virtual	int	close ( const Message& );

private:
	View *		nextTab ( View * );
	View *		prevTab ( View * );
	void		moveFocusVert ( int dir );
	void		moveFocusHorz ( int dir );
	int		getDistance ( View *, View * );
};

extern	Window  * activeWindow;
extern	Font	* sysFont;
extern	Font	* sysFixedFont;
extern	Surface	* closeBox [2];
extern	Surface	* sysCheckBox [3];
extern	Surface * sysRadioButton [3];
extern	Surface	* sysArrows [8];
extern	Surface	* marker;
extern	Surface	* ind [2];

void	initWindows ();
void	doneWindows ();

Object * loadDeskTop ();
Object * loadWindow ();
Object * loadDialogWindow ();

#endif
