//
// Simple windowing system, basic class for all window objects
// Author: Alex V. Boreskoff
// Last update: 05/05/1999
//
#ifndef __VIEW__
#define __VIEW__

#include        <string.h>
#include        "point.h"
#include        "rect.h"
#include        "mouse.h"
#include        "surface.h"
#include        "object.h"
#include        "message.h"

#define IDOK            1
#define IDCANCEL        2
#define IDHELP          3
#define	IDOPEN		4
#define	IDCLOSE		5
#define	IDSAVE		6
#define	IDQUIT		7
				// generic notifications (send via WM_COMMAND)
#define VN_TEXTCHANGED  0       // view text has changed
#define VN_FOCUS        1       // view received/lost focus

                                // style bits
#define WS_ACTIVATEABLE         0x0001  // can be activated (for Windows only)
#define WS_REPAINTONFOCUS       0x0002  // should be repainted on
                                        // focus change
#define WS_FLOATING             0x0004  // window floats above normal ones

                                // status bits
#define WS_ENABLED              0x0001  // can receive focus, mouse & kbd messages
#define WS_VISIBLE              0x0002  // is visible (shown)
#define WS_COMPLETELYVISIBLE    0x0004  // not overlaid by someone

                                // hit codes
#define HT_CLIENT       0

class   View;
class   Menu;
class   Map;

extern  Surface * screenSurface;        // surface we draw on
extern  Rect      screenRect;           // current screen rect
extern  View    * deskTop;              // desktop
extern  View    * focusedView;          // focused view

////////////////////////////////////////////////////////////////////////////
int     setFocus      ( View * );
void    redrawRect    ( Rect& );
View  * findView      ( const Point& );
///////////////////////////////////////////////////////////////////////////

class   View : public Object                    // base class of all windowing system
{
protected:
        char  * text;                           // text or caption
        View  * parent;                         // owner of this view
        View  * next;                           // next child of parent
        View  * prev;                           // previous chlid of this parent
        View  * child;                          // topmost child view of this
        int     style;                          // view style
        int     status;                         // view status
        Rect    area;                           // frameRect of this view
        int     tag;                            // tag of view (-1 by default)
        int     lockCount;
        View  * delegate;                       // to whom send notifications
        View  * hook;                           // view that hooked this one
public:
	View ( int x, int y, int w, int h, View * owner = NULL );
	View ();
        virtual ~View ();

        virtual char  * getClassName () const
        {
                return "View";
        }

        virtual int     put ( Store * ) const;
        virtual int     get ( Store * );

	virtual void    init () {}              // post-constructor init

        virtual void    show ();                // show this window
        virtual void    hide ();                // hide this window

        virtual int     handle ( const Message& );
                                                // keyboard messages
        virtual int     keyDown ( const Message& );
        virtual int     keyUp ( const Message& );
                                                // mouse events
        virtual int     mouseDown ( const Message& );
        virtual int     mouseUp ( const Message& );
        virtual int     mouseMove ( const Message& );
        virtual int     rightMouseDown ( const Message& );
        virtual int     rightMouseUp ( const Message& );
        virtual int     mouseDoubleClick ( const Message& );
        virtual int     mouseTripleClick ( const Message& );

        virtual int     receiveFocus ( const Message& );
        virtual int     looseFocus ( const Message& );

        virtual int     command ( const Message& );
        virtual int     timer ( const Message& );
        virtual int     close ( const Message& );

        virtual void    helpRequested ( const Message& ) {}

        virtual void    draw ( const Rect& ) const {}

        virtual void    getMinMaxSize ( Point& minSize, Point& maxSize ) const;

        virtual Rect    getClientRect ( Rect& ) const;

        virtual int     hitTest ( const Point& ) const
        {
                return HT_CLIENT;
        }

        virtual Menu  * getMenu ( const Message& ) const
        {
                return NULL;
        }

        virtual int     handleHooked ( const Message& )
        {
                return FALSE;                   // message not processed
                                                // by hook, so own message
                                                // handler should be called
        }
						// whether the view can receive
	virtual int     canReceiveFocus () const// input focus
	{
		return TRUE;
	}

	void    setTag ( int newTag )
	{
		tag = newTag;
        }

        View  * viewWithTag ( int theTag );
        void    addSubView ( View * subView );
        void    setFrame ( int x, int y, int width, int height );
        void    setFrame ( const Rect& r )
        {
                setFrame ( r.x1, r.y1, r.width (), r.height () );
        }

        Rect    getFrame () const
	{
                return area;
        }

        Rect    getScreenRect () const;
        Point&  local2Screen ( Point& ) const;
        Rect&   local2Screen ( Rect& ) const;
        Point&  screen2Local ( Point& ) const;
        Rect&   screen2Local ( Rect& ) const;

        void    enableView ( int = TRUE );

        void    beginDraw () const;
        void    endDraw   () const;
        void    repaint   () const;
	void    lock      ( int flag = TRUE );

	View  * getFirstTab () const;

	void    setZOrder ( View * behind = NULL );
	View  * hookWindow ( View * whome );

	int     containsFocus () const;

	char  * getText () const
	{
		return text;
	}

	char  * setText ( const char * newText );

	View  * getParent () const
	{
		return parent;
	}

	int     getStyle () const
	{
		return style;
	}

	void    setStyle ( int newStyle )
	{
		style = newStyle;
	}

	int	getStatus () const
	{
		return status;
	}

	int     contains ( const Point& p )
	{
		return p.x >= 0 && p.x < width () && p.y >= 0 && p.y < height ();
	}

	int     width () const
	{
		return area.width ();
	}

	int     height () const
	{
		return area.height ();
	}

	int     isVisible () const
	{
		return status & WS_VISIBLE;
	}

	int     isActivateable () const
	{
		return style & WS_ACTIVATEABLE;
	}

	int     isFloating () const
	{
		return style & WS_FLOATING;
	}

	int     isEnabled () const
	{
		return status & WS_ENABLED;
	}

	int     isLocked () const
	{
		return lockCount > 0;
	}

	int     isFocused () const
	{
		return focusedView == this;
	}

	friend class Map;
	friend class DialogWindow;
	friend int   setFocus ( View * );
};

Object * loadView ();

///////////////////////////////////////////////////////////////////////////

#endif
