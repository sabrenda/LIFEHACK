//
// Simple windowing system, basic class for all window objects
// Author: Alex V. Boreskoff
// Last update: 05/05/1999
//
#include        <assert.h>
#include        <dos.h>
#include        <stdlib.h>

#include        "array.h"
#include        "store.h"
#include        "view.h"

////////////////////////// Global variables  //////////////////////////////

View * deskTop           = NULL;
View * focusedView       = NULL;

Array  floaters ( 10, 10 );

///////////////////// Objects to manage screen layout  ////////////////////

class   Region : public Rect
{
public:
	View   * owner;
	Region * next;
};

class   Map
{
	Region  * pool;                 // pool of regions
	int       poolSize;             // # of structures allocated
	Region  * firstAvailable;       // pointer to 1st struct after all allocated
	Region  * lastAvailable;        // last item in the pool
	Region  * free;                 // pointer to free structs list (below firstAvailable)
	Region  * start;                // first region in the list

public:
	Map ( int mapSize )
	{
		pool           = new Region [poolSize = mapSize];
		firstAvailable = pool;
		lastAvailable  = pool + poolSize - 1;
		free           = NULL;
		start          = NULL;
	}

	~Map ()
	{
		delete [] pool;
	}

	void    freeAll ()              // free all used regions
	{
		firstAvailable = pool;
		free           = NULL;
		start          = NULL;
	}

	Region  * allocRegion ();       // allocate unused region
					// deallocate region (return to free list)
	void    freeRegion ( Region * reg )
	{
                reg -> next = free;
                free        = reg;
        }

        void     rebuildMap   ();               // rebuild map from beginning
                                                // redraw given screen rectangle
        void     redrawRect   ( const Rect& ) const;
                                                // find window, containg point
        View   * findView   ( const Point& ) const;
                                                // find 1st region of view list
        Region * findViewArea ( const View * view ) const;
                                                // add view (with all subviews)
						// to screen map
        void     addView ( View *, Rect&, int  );

        friend  class View;
};

/////////////////////////////// Map methods ////////////////////////////////

Region * Map :: allocRegion ()
{                                       // when no free spans
        if ( free == NULL )
                if ( firstAvailable <= lastAvailable )
                        return firstAvailable++;
                else
                        return NULL;

        Region * res = free;

        free = free -> next;

        return res;
}

void    Map :: rebuildMap ()
{
        freeAll ();

	addView ( deskTop, screenRect, 0 );
}

void    Map :: addView ( View * view, Rect& viewRect, int recLevel )
{
	int	 updatePrev;			// whether we should update prev
	Rect     r;
	Rect     splittingRect;
	View   * owner;
	Region * reg, * next;
	Region * prev = NULL;

	if ( !view -> isVisible () )            // if not vivible nothing to add
		return;

	viewRect       &= screenRect;           // clip area to screen
	view -> status |= WS_COMPLETELYVISIBLE; // initially not overlapped by anyone

	if ( viewRect.isEmpty () )
		return;

	for ( reg = start; reg != NULL; reg = next )
	{
		next       = reg -> next;
		updatePrev = TRUE;
						// floated views should be added
						// after all subviews
		if ( reg -> owner -> isFloating () )
		{
			if ( recLevel >= 0 )    // if adding view then store floater
			{
				floaters.insert ( reg -> owner );

				prev = reg;	// update prev

				continue;       // skip floater
			}
		}

		splittingRect  = * reg;         // get current rect
		splittingRect &= viewRect;      // clip it to viewRect

		if ( splittingRect.isEmpty () ) // if not intersection
		{
			prev = reg;		// update prev

			continue;               // then continue
		}

		if ( splittingRect == * reg )   // current is overlapped
		{
			if ( prev != NULL )     // remove region from list
				prev -> next = next;

			freeRegion ( reg );     // free it

			updatePrev = FALSE;
		}
		else
		{
			r     = * reg;          // save current rect
			owner = reg -> owner;   // and it's owner
			owner -> status &= ~WS_COMPLETELYVISIBLE;

						// 1st block
			reg -> y2 = splittingRect.y1 - 1;

			if ( !reg -> isEmpty () )
			{
				prev         = reg;
				reg          = allocRegion ();
				prev -> next = reg;
			}

						// 2nd block
			reg -> x1    = r.x1;
			reg -> y1    = splittingRect.y1;
			reg -> x2    = splittingRect.x1 - 1;
			reg -> y2    = splittingRect.y2;
			reg -> owner = owner;

			if ( !reg -> isEmpty () )
			{
				prev         = reg;
				reg          = allocRegion ();
				prev -> next = reg;
			}

						// 3rd block
			reg -> x1    = splittingRect.x2 + 1;
			reg -> y1    = splittingRect.y1;
			reg -> x2    = r.x2;
			reg -> y2    = splittingRect.y2;
			reg -> owner = owner;

			if ( !reg -> isEmpty () )
			{
				prev         = reg;
				reg          = allocRegion ();
				prev -> next = reg;
			}

						// 4th block
			reg -> x1    = r.x1;
			reg -> y1    = splittingRect.y2 + 1;
			reg -> x2    = r.x2;
			reg -> y2    = r.y2;
			reg -> owner = owner;

			if ( reg -> isEmpty () )// remove from chain
			{
				if ( prev != NULL )
					prev -> next = next;

				freeRegion ( reg );

				updatePrev = FALSE;
			}
			else
				reg -> next  = next;
		}

		if ( updatePrev )
			prev = reg;
	}
					// now append viewRect to the end of list
	reg = allocRegion ();           // allocate new block & add it

	* (Rect *) reg = viewRect;
	reg -> owner   = view;
	reg -> next    = NULL;

	if ( prev != NULL )             // append it to the end of list
		prev -> next = reg;
	else
		start = reg;

	if ( view -> isFloating () && recLevel == 0 )
		floaters.insert ( view );

	if ( view -> child != NULL )    // now add it's subviews
	{                               // find downmost view
		for ( View * c = view -> child; c -> prev != NULL; )
			c = c -> prev;
					// start adding from downmost subview
		for ( ; c != NULL; c = c -> next )
		{                       // get subview's rect
			r  = c -> getScreenRect ();
			r &= viewRect;  // clip to parent's rect
					// add to map
			addView ( c, r, recLevel + 1 );
		}
	}

	if ( recLevel == 0 )
	{
		for ( int i = 0; i < floaters.getCount (); i++ )
		{
			View * v = (View *) floaters [i];

			addView ( v, v -> getScreenRect (), -10000 );
		}

		floaters.deleteAll ();
	}
}

void    Map :: redrawRect ( const Rect& r ) const
{
	if ( r.isEmpty () )
		return;

	screenSurface -> beginDraw ();

	for ( Region * reg = start; reg != NULL; reg = reg -> next )
	{
		Rect    clipRect ( * reg );
		Point   org ( 0, 0 );

		clipRect &= r;

		if ( clipRect.isEmpty () )
			continue;

		screenSurface -> setClipRect ( clipRect );

		Rect    drawRect ( clipRect );

		reg -> owner -> screen2Local ( drawRect );
		reg -> owner -> beginDraw ();
		reg -> owner -> draw ( drawRect );
		reg -> owner -> endDraw ();
	}

	screenSurface -> endDraw ();
}

View *  Map :: findView ( const Point& p ) const
{
        for ( register Region * reg = start; reg != NULL; reg = reg -> next )
                if ( reg -> contains ( p ) )
			return reg -> owner;

        return NULL;
}

Region * Map :: findViewArea ( const View * view ) const
{
        for ( Region * reg = start; reg != NULL; reg = reg -> next )
                if ( reg -> owner == view )
                        return reg;

	return NULL;
}

////////////////////////////////////////////////////////////////////////////

static  Map      screen ( 2000 );

////////////////////////////  View methods  //////////////////////////////

View :: View ( int x, int y, int w, int h, View * p ) : area ( x, y, x + w - 1, y + h - 1 )
{
	style     = 0;
	status    = 0;
	lockCount = 0;          // initially not locked
	tag       = -1;         // default tag
	child     = NULL;
	prev      = NULL;
	next      = NULL;
	delegate  = p;	     	// by default all notifications are sent to parent
	hook      = NULL;
	text      = (char *) malloc ( 80 );

	strcpy ( text, "" );

	if ( ( parent = p ) != NULL )
		parent -> addSubView ( this );
}

View :: View ()
{
	parent = deskTop;
	text   = (char *) malloc ( 80 );
}

View :: ~View ( )
{
        hide ();

        while ( child != NULL )         // remove all subwindows
                delete child;

        if ( parent != NULL )
                if ( parent -> child == this )
                        parent -> child = prev;

        if ( prev != NULL )             // remove from chain
		prev -> next = next;

        if ( next != NULL )
                next -> prev = prev;

        free ( text );
}

int     View :: put ( Store * s ) const
{
        s -> putString ( text );
	s -> putInt    ( style );
	s -> putInt    ( status );
	s -> putInt    ( area.x1 );
	s -> putInt    ( area.y1 );
	s -> putInt    ( area.x2 );
	s -> putInt    ( area.y2 );
	s -> putInt    ( tag );

	int     subViewCount = 0;

	for ( View * v = child; v -> prev != NULL; v = v -> prev, subViewCount++ )
		;

	s -> putInt ( subViewCount + 1 );

	for ( ; v != NULL; v = v -> next )
		s -> putObject ( v );

	return TRUE;
}

int     View :: get ( Store * s )
{
	text    = s -> getString ();
	style   = s -> getInt    ();
	status  = s -> getInt    () & ~WS_VISIBLE;
	area.x1 = s -> getInt    ();
	area.y1 = s -> getInt    ();
	area.x2 = s -> getInt    ();
	area.y2 = s -> getInt    ();
	tag     = s -> getInt    ();

	int     subViewCount = s -> getInt ();

	for ( int i = 0; i < subViewCount; i++ )
		addSubView ( (View *) s -> getObject () );

	return TRUE;
}

void    View :: show ()
{
	if ( !isVisible () )
	{
		status |= WS_VISIBLE;

		screen.addView ( this, getScreenRect (), 0 );
		repaint ();

		if ( deskTop == NULL )
			deskTop = this;
	}

	if ( isEnabled () )
		setFocus ( this );
}

void    View :: hide ()
{
        if ( isVisible () )
        {
                if ( containsFocus () ) // move focus from this view or it's children
		{
			for ( View * v = prev; v != NULL; v = v -> prev )
				if ( v -> isVisible () && v -> isEnabled () )
					break;

			setFocus ( v != NULL ? v : parent );
		}

		status &= ~WS_VISIBLE;
					// hide all children
		for ( View * v = child; v != NULL; v = v -> prev )
		{
			v -> lockCount ++;
			v -> hide ();
			v -> lockCount --;
		}

		if ( !isLocked () )
		{
			screen.rebuildMap ();
			screen.redrawRect ( getScreenRect () );
		}

		if ( deskTop == this )
			deskTop = NULL;
	}
}

char  * View :: setText ( const char * newText )
{
	free ( text );

	text = strdup ( newText );

        repaint ();

        sendMessage ( delegate, WM_COMMAND, VN_TEXTCHANGED, 0, this );

        return text;
}

int     View :: handle ( const Message& m )
{					// check whether hook
					// intercepts message
	if ( hook != NULL && hook -> handleHooked ( m ) )
		return TRUE;

        switch ( m.code )
        {
                case WM_KEYDOWN:
                        return keyDown ( m );

                case WM_KEYUP:
                        return keyUp ( m );

		case WM_MOUSEMOVE:
                        return mouseMove ( m );

                case WM_LBUTTONDOWN:
                        return mouseDown ( m );

                case WM_LBUTTONUP:
                        return mouseUp ( m );

                case WM_RBUTTONDOWN:
                        return rightMouseDown ( m );

                case WM_RBUTTONUP:
                        return rightMouseUp ( m );

                case WM_DBLCLICK:
                        return mouseDoubleClick ( m );

                case WM_TRIPLECLICK:
                        return mouseTripleClick ( m );

                case WM_RECEIVEFOCUS:
                        return receiveFocus ( m );

                case WM_LOOSEFOCUS:
                        return looseFocus ( m );

                case WM_COMMAND:
                        return command ( m );

                case WM_TIMER:
                        return timer ( m );

                case WM_CLOSE:
                        return close ( m );
        }

        return FALSE;                           // unknown message - not processed
}

int     View :: keyDown ( const Message& m )    // let parent process it
{
	return parent != NULL ? parent -> keyDown ( m ) : FALSE;
}

int     View :: keyUp ( const Message& m )      // let parent process it
{
	return parent != NULL ? parent -> keyUp ( m ) : FALSE;
}

int     View :: mouseDown ( const Message& m )  // let parent process it
{
	return parent != NULL ? parent -> mouseDown ( m ) : FALSE;
}

int     View :: mouseUp ( const Message& m )    // let parent process it
{
	return parent != NULL ? parent -> mouseUp ( m ) : FALSE;
}

int     View :: mouseMove ( const Message& m )  // let parent process it
{
	return parent != NULL ? parent -> mouseMove ( m ) : FALSE;
}

int     View :: rightMouseDown ( const Message& m )     // let parent process it
{
	return parent != NULL ? parent -> rightMouseDown ( m ) : FALSE;
}

int     View :: rightMouseUp ( const Message& m )       // let parent process it
{
	return parent != NULL ? parent -> rightMouseUp ( m ) : FALSE;
}

int     View :: mouseDoubleClick ( const Message& m )   // let parent process it
{
	return parent != NULL ? parent -> mouseDoubleClick ( m ) : FALSE;
}

int     View :: mouseTripleClick ( const Message& m )   // let parent process it
{
	return parent != NULL ? parent -> mouseTripleClick ( m ) : FALSE;
}

int     View :: receiveFocus ( const Message& m )
{
	sendMessage ( delegate, WM_COMMAND, VN_FOCUS, TRUE, this );

	if ( style & WS_REPAINTONFOCUS )
		repaint ();

	return TRUE;                                    // message processed
}

int     View :: looseFocus ( const Message& m )
{
	sendMessage ( delegate, WM_COMMAND, VN_FOCUS, FALSE, this );

	if ( style & WS_REPAINTONFOCUS )
		repaint ();

	return TRUE;                                    // message processed
}

int     View :: command ( const Message& m )
{
	return FALSE;                                   // not processed
}

int     View :: timer ( const Message& m )
{
	return FALSE;
}

int     View :: close ( const Message& m )
{
	hide        ();
	autorelease ();

	return TRUE;                                    // processed
}

void    View :: getMinMaxSize ( Point& minSize, Point& maxSize ) const
{
	minSize.x = 0;
	minSize.y = 0;
	maxSize.x = MAXINT;
	maxSize.y = MAXINT;
}

Rect    View :: getClientRect ( Rect& client ) const
{
        client.x1 = 0;
        client.y1 = 0;
        client.x2 = width () - 1;
        client.y2 = height () - 1;

        return client;
}

View  * View :: viewWithTag ( int theTag )
{
	if ( tag == theTag )
		return this;

	View * res = NULL;

	for ( View * v = child; v != NULL; v = v -> prev )
		if ( ( res = v -> viewWithTag ( theTag ) ) != NULL )
			return res;

	return res;
}

void    View :: addSubView ( View * subView )
{
	if ( child != NULL )
		child -> next = subView;

	subView -> prev = child;
	child           = subView;
}

void    View :: setFrame ( int x, int y, int width, int height )
{
        Rect    r ( x, y, x + width - 1, y + height - 1 );

        if ( r != area )
        {
                Rect    updateRect = area;

                updateRect |= r;
                area        = r;

                if ( parent != NULL )
                        parent -> local2Screen ( updateRect );

                screen.rebuildMap ();
                screen.redrawRect ( updateRect );
	}
}

Rect    View :: getScreenRect () const
{
        Rect    r ( area );

        if ( parent != NULL )
                parent -> local2Screen ( r );

        return r;
}

Point&  View :: local2Screen ( Point& p ) const
{
        for ( View * w = (View *) this; w != NULL; w = w -> parent )
        {
                p.x += w -> area.x1;
                p.y += w -> area.y1;
        }

        return p;
}

Rect&   View :: local2Screen ( Rect& r ) const
{
        for ( View * w = (View *) this; w != NULL; w = w -> parent )
                r.move ( w -> area.x1, w -> area.y1 );

        return r;
}

Point&  View :: screen2Local ( Point& p ) const
{
        for ( View * w = (View *) this; w != NULL; w = w -> parent )
        {
                p.x -= w -> area.x1;
                p.y -= w -> area.y1;
        }

	return p;
}

Rect&   View :: screen2Local ( Rect& r ) const
{
        for ( View * w = (View *) this; w != NULL; w = w -> parent )
                r.move ( - w -> area.x1, - w -> area.y1 );

        return r;
}

void    View :: enableView ( int flag )
{
        if ( flag )
                status |= WS_ENABLED;
        else
                status &= ~WS_ENABLED;
}

void    View :: beginDraw () const
{
        Rect    clipRect ( getScreenRect () );
        Point   org ( 0, 0 );

        clipRect &=  screenRect;

        local2Screen            ( org );
        screenSurface -> setOrg ( org );
	hideMouseCursor         ( /*clipRect*/ );
}

void    View :: endDraw () const
{
        showMouseCursor ();
}

void    View :: repaint () const
{
        if ( isVisible () && !isLocked () )
        {
		Rect    clipRect ( getScreenRect () );

                clipRect &= screenRect;

                if ( style & WS_COMPLETELYVISIBLE )
                {
                        Rect    drawRect ( 0, 0, width () - 1, height () - 1 );

			screenSurface -> beginDraw ();
			beginDraw ();
			draw ( drawRect );
			endDraw ();
			screenSurface -> endDraw ();
                }
                else
                        screen.redrawRect ( clipRect );
        }
}

void    View :: lock ( int flag )
{
        if ( flag )
                lockCount++;
        else
                lockCount--;
}

View *  View :: getFirstTab () const
{
        if ( child == NULL )
                return NULL;

        for ( View * w = child; w -> prev != NULL; w = w -> prev )
                        ;

        while ( w != NULL && ! ( w -> isVisible () && w -> isEnabled () ) )
                w = w -> next;

        return w;
}

void    View :: setZOrder ( View * behind )
{
        if ( prev != NULL )             // remove from chain
                prev -> next = next;

        if ( next != NULL )
                next -> prev = prev;

	if ( parent != NULL && parent -> child == this )
		parent -> child = prev;

	if ( behind == NULL )           // set above all children
        {
                if ( parent != NULL )
                {
			if ( parent -> child != this )
			{
				parent -> child -> next = this;
				prev                    = parent -> child;
				next                    = NULL;
				parent -> child         = this;
			}
		}
	}
	else
	{
		if ( ( prev = behind -> prev ) != NULL )
			behind -> prev -> next = this;

		next           = behind;
		behind -> prev = this;
	}

	screen.rebuildMap ();
	screen.redrawRect ( getScreenRect () );
}

View  * View :: hookWindow ( View * whome )
{
	View * oldHook = whome -> hook;

        whome -> hook = this;

        return oldHook;
}

int     View :: containsFocus () const
{
        for ( View * w = focusedView; w != NULL; w = w -> parent )
                if ( w == this )
                        return TRUE;

        return FALSE;
}

///////////////////////////////////////////////////////////////////////////

int     setFocus ( View * view )
{
	if ( focusedView == view )              // check if already focused
		return TRUE;
						// check whether we can set focus
						// to this view (it's visible & enabled)
	for ( View * v = view; v != NULL; v = v -> parent )
		if ( ! v -> isVisible () )
			return FALSE;           // cannot set focus on invisible window

	if ( view != NULL && ! view -> isEnabled () )
		return FALSE;                   // cannot set focus on disabled window

	if ( ! view -> canReceiveFocus () )     // view does not want to be input focus
		return FALSE;
						// inform current focus view
						// that's it's loosing focus to 'view'
	View * oldFocus = focusedView;		// save focus

	focusedView = NULL;			// so isFocused return FALSE

	sendMessage ( oldFocus, WM_LOOSEFOCUS );// inform focused view that it's loosing input focus
						// inform window that it gets input focus
	sendMessage ( focusedView = view, WM_RECEIVEFOCUS );

	return TRUE;
}

void    redrawRect ( const Rect& r )
{
        screen.redrawRect ( r );
}

View * findView ( const Point& p )
{
        return screen.findView ( p );
}

Object * loadView ()
{
	return new View;
}
