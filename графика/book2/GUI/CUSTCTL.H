#ifndef	__CUSTCTL__
#define	__CUSTCTL__

#include "Win.h"
#include "BitMap.h"
#include "Font.h"
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

			// button status
#define	BS_PRESSED		0x0008
#define	BS_CHECKED		0x0010
#define	BS_TRACKING		0x0020

			// scroll bar status
#define	SBS_UPPRESSED		0x0008
#define	SBS_DOWNPRESSED		0x0010
#define	SBS_BUTTONPRESSED	0x0020

			// frame style bits
#define	FS_RAISED		0x0004

			// item states
#define	IS_SELECTED		0x0001
#define	IS_FOCUSED		0x0002
#define	IS_PRESSED		0x0004

			// system controls sizes
#define	SM_CHECKBOXSIZE	  12
#define	SM_RADIOSIZE	  12
#define	SM_INDICATOR	  9
#define	SM_SCROLLBARSIZE  20

			// scroll bar hit codes
#define	SB_NOACTION	110
#define	SB_LINEUP	101
#define	SB_LINEDOWN	102
#define	SB_PAGEUP	103
#define	SB_PAGEDOWN	104
#define	SB_TRACK	105
#define	SB_THUMBTRACK	106
#define	SB_THUMBPOS	107

				// editline notification codes
#define	LEN_TEXTCHANGED	300

				// list box notification codes
#define	LBN_SELCHANGE	200
#define	LBN_DBLCLICK	201
#define	LBN_CHECKCHANGE	202

///////////////////////////////////////////////////////////////////////////

class	Caret
{
public:
	HWND	wnd;		// owner ot caret
	int	x, y;           // coordinates
	int	on;		// whether it's on or not
	int	lockCount;	// whether this is locked
	int	height;

	Caret ( int, HWND );

	void	Show ();
	void	Hide ();
	void	Move ( int, int );
	void	Flash ();

private:
	void	Draw ();
};
				// next 6 static controls can never receive focus
				// since they are not enabled
class	StaticPic : public GenericWindow
{
public:
	BitMap	* pic;

	StaticPic ( int x, int y, HWND p, BitMap * bm ) : GenericWindow ( x, y, bm -> Width, bm -> Height, p )
	{
		pic     = bm;
		status |= WS_VISIBLE;
	}

	~StaticPic ()
	{
		delete pic;
	}

	virtual	void	Paint ()
	{
		HideMouseCursor ();
		pic -> Put ( 0, 0 );
		ShowMouseCursor ();
	}
};

class	HorzLine : public GenericWindow
{
public:
	HorzLine ( int x, int y, int w, HWND p ) : GenericWindow ( x, y, w, 2, p )
	{
		status |= WS_VISIBLE;
		strcpy ( text, "HorzLine" );
	}

	virtual	void	Paint ();
};

class	VertLine : public GenericWindow
{
public:
	VertLine ( int x, int y, int h, HWND p ) : GenericWindow ( x, y, 2, h, p )
	{
		status |= WS_VISIBLE;
		strcpy ( text, "VertLine" );
	}

	virtual	void	Paint ();
};

class	Frame : public GenericWindow
{
public:
	Frame ( int x, int y, int w, int h, HWND p, char * t, int st ) : GenericWindow ( x, y, w, h, p )
	{
		strcpy ( text, t );
		status |= WS_VISIBLE;
		style  |= st;
	}

	virtual	void	Paint ();
};

class	StaticText : public GenericWindow
{
public:
	Font	* fnt;			// font used to draw text
	char	  backColor, textColor;	// colors for background & text
	char	  borderType;
	int	  shouldDelete;

	StaticText ( int x, int y, int w, int h, HWND p, char * t, char tc, char bc, char br = SS_3D, Font * fn = SysFont, int del = FALSE );
	~StaticText ()
	{
		if ( shouldDelete )
			delete fnt;
	}

	virtual	void	Paint ();
};

class	Indicator : public GenericWindow
{
public:
	long	maxValue;
	long	curValue;
	int	numLights;
	int	activeLights;

	Indicator ( int, int, int, long, HWND );

	virtual	void	Paint ();
	void		SetValue ( long );
};

class	EditLine : public StaticText
{
public:
	int	insState;
	int	pos;
	int	len;
	Caret * caret;

	EditLine ( int x, int y, int w, int h, HWND p, char * t, char bc, char tc, char br = SS_3D, Font * fn = SysFont, int del = FALSE );
	~EditLine ()
	{
		delete caret;
	}

	virtual	void	Paint ();
	virtual	long	Handle ( Message& );
	virtual	int	QueryUseKey ( int );
	virtual	int	Filter ( char ) { return TRUE; };
	int		GetPos ( int );
	void		SetPos ( int );
};

class	PushButton : public GenericWindow
{
public:
	int	id;

	PushButton ( int x, int y, int w, int h, HWND p, int anId ) : GenericWindow ( x, y, w, h, p )
	{
		id      = anId;
		status |= WS_VISIBLE | WS_ENABLED;
	}

	virtual	int	QueryUseKey ( int );
	virtual	long	Handle ( Message& );
};

class	TextButton : public PushButton
{
public:
	TextButton ( int x, int y, int w, int h, HWND p, int anId, char * t ) : PushButton ( x, y, w, h, p, anId )
	{
		strcpy ( text, t );
	}

	virtual	void	Paint ();
};

class	PicButton : public PushButton
{
public:
	BitMap * face [3];		// normal, pressed, focused

	PicButton ( int, int, HWND, BitMap *, BitMap *, BitMap *, int );
	~PicButton ();

	virtual	void	Paint ();
};

class	CheckBox : public GenericWindow
{
public:
	int	id;

	CheckBox ( int x, int y, int w, int h, HWND p, int ident, char * t, int st = 0 ) : GenericWindow ( x, y, w, h, p )
	{
		id      = ident;
		style  |= WS_REPAINTONFOCUS | st;
		status |= WS_VISIBLE | WS_ENABLED;
		strcpy ( text, t );
	}

	virtual	int	QueryUseKey ( int );
	virtual	void	Paint ();
	virtual	long	Handle ( Message& );
};

class	RadioGroup : public GenericWindow
{
public:
	int	id;
	int	value;		//
	int	rows;		// # of rows
	int	cols;		// # of	cols
	int	subFocus;
	int	cellWidth;
	int	cellHeight;

	RadioGroup ( int x, int y, int nRows, int nCols, int cw, int ch, int ident, int v, char * s , HWND p );

	virtual	int	QueryUseKey ( int );
	virtual	void	Paint   ();
	virtual	long	Handle  ( Message& );
	virtual	int	HitTest ( Point& );
};

class	ScrollBar : public GenericWindow
{
public:
	int	minValue, maxValue;
	int	curValue;

	ScrollBar ( int x, int y, int w, int h, HWND p ) : GenericWindow ( x, y, w, h, p )
	{
		partCode = SB_NOACTION;
	}

	virtual	long	Handle ( Message& );
	virtual	int	ScrollerLen () = 0;

	void	SetValue ( int );
	void	SetRange ( int, int, int );
	void	Up ()
	{
		SetValue ( curValue - 1 );
	}
	void	Down ()
	{
		SetValue ( curValue + 1 );
	}
	void	PgUp ()
	{
		SetValue ( curValue - pageSize );
	}
	void	PgDown ()
	{
		SetValue ( curValue + pageSize );
	}


protected:
	int	pageSize;
	int	markerSize;			// in pixels
	int	markerPos;			// in pixels
	int	partCode;
	int	scrollMsg;			// message to be sent to parent
	int	upButton;			// 0 for VScroll, 2 for HScroll
	int	downButton;			// 1 for VScroll, 3 for HScroll

	void	ComputeMarker ();
	void	RepaintButton ( int, int );
	void	Track ( Point& );
};

class	VScrollBar : public ScrollBar
{
public:
	VScrollBar ( int, int, int, HWND );

	virtual	int	ScrollerLen ();
	virtual	int	HitTest ( Point& );
	virtual	void	Paint ();
};

class	HScrollBar : public ScrollBar
{
public:
	HScrollBar ( int, int, int, HWND );

	virtual	int	ScrollerLen ();
	virtual	int	HitTest ( Point& );
	virtual	void	Paint ();
};
/*
class	ListBox : public GenericWindow
{
public:
	ScrollBar  * Scroller;
	int	     Top;
	int	     Current;
	int          ItemHeight;
	int	     PageSize;

	ListBox ( int, int, int, int, HWND, int );

	virtual	void	Paint ();
	virtual	int	Handle ( Message& );
	virtual	void	DrawItem ( int, int, int, int ) = 0;
	virtual	int	GetCount () = 0;
	virtual	int	FindItem ( Point& );

	void	SetActive ( int );
	void	ResetScroller ();
};
*/
void	DrawBox ( int, int, int, int, char *, unsigned, int, BitMap * );

#endif