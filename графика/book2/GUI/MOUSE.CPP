//
// Mouse support for VESA-modes
// Author: Alex V. Boreskoff
//
#include        <dos.h>
#include        <i86.h>
#include        "mouse.h"
#include        "message.h"
#include        "video.h"

///////////////////////// static data /////////////////////////////////

static  Rect            hideRange;              // rect to hide cursor in
static  int             hideRangeDefined;
static  MouseState      curMouseState;
static  CursorShape   * curCursor;
static	Point   	dstPt ( 0, 0 );		// destination in saveSurface
static	Rect		saveRect;		// rect to saved in saveSurface
static	Rect		copyRect;		// rect to be copied to screenSurface
static	Point		lastClickPoint ( 0, 0 );// where last click happened
static	int		lastClickTime   = 0;
static	int		doubleClickFlag = 0;
static  int             mouseHidden     = 1;
static  int             mouseFreeze     = 0;
static  Surface       * saveSurface     = NULL; // where image under cursor is saved
static  View          * mouseOwner      = NULL; // who captured the mouse
CursorShape           * arrowCursor     = NULL;
CursorShape           * waitCursor      = NULL;

/////////////////////////// functions //////////////////////////////////

void showMouseCursor ()
{
	mouseFreeze++;

        if ( mouseHidden <= 1 )
                if ( hideRangeDefined )
                        hideRangeDefined--;
                else
                {
			Rect    saveClip ( screenSurface -> getClipRect () );
			Point	saveOrg  ( screenSurface -> getOrg () );
			Rect    saveRect ( curMouseState.loc.x, curMouseState.loc.y,
					   curMouseState.loc.x + 32, curMouseState.loc.y + 32 );

			screenSurface -> setClipRect ( screenRect );
			screenSurface -> org.x = 0;
			screenSurface -> org.y = 0;
			screenSurface -> copy ( *saveSurface, saveRect, dstPt );

			if ( curCursor -> image != NULL )
				curCursor -> image -> copyTransp ( *screenSurface, curMouseState.loc, curCursor -> transpColor );

			mouseHidden = 0;

			screenSurface -> setClipRect ( saveClip );
			screenSurface -> setOrg      ( saveOrg );
                }
        else
                mouseHidden--;

        mouseFreeze--;
}

void hideMouseCursor ()
{
	mouseFreeze++;

	if ( mouseHidden++ < 1 )
	{
		Rect    saveClip ( screenSurface -> getClipRect () );
		Point	saveOrg  ( screenSurface -> getOrg () );

		screenSurface -> setClipRect ( screenRect );
		screenSurface -> org.x = 0;
		screenSurface -> org.y = 0;
		saveSurface   -> copy ( * screenSurface, curMouseState.loc );
		screenSurface -> setClipRect ( saveClip );
		screenSurface -> setOrg ( saveOrg );
	}

        mouseFreeze--;
}

void hideMouseCursor ( const Rect& r )
{
        mouseFreeze++;

	if ( hideRangeDefined )
                hideRange |= r;
	else
		hideRange = r;

        hideRangeDefined++;

        if ( !mouseHidden && hideRange.contains ( curMouseState.loc ) )
        {
		hideMouseCursor ();

                mouseHidden     += hideRangeDefined - 1;
                hideRangeDefined = 0;
        }

        mouseFreeze--;
}

void readMouseState ( MouseState& state )
{
	mouseFreeze++;

	state = curMouseState;

	mouseFreeze--;
}

void    setHorzMouseRange ( int xmin, int xmax )
{
	union REGS inRegs, outRegs;

	inRegs.w.ax = 7;
	inRegs.w.cx = xmin;
	inRegs.w.dx = xmax;


	int386 ( 0x33, &inRegs, &outRegs );
}

void    setVertMouseRange ( int ymin, int ymax )
{
	union REGS inRegs, outRegs;

	inRegs.w.ax = 8;
	inRegs.w.cx = ymin;
	inRegs.w.dx = ymax;

	int386 ( 0x33, &inRegs, &outRegs );
}

void setMouseShape ( CursorShape * c )
{
	if ( c == curCursor )
                return;

        hideMouseCursor ();

        curCursor = c;

        showMouseCursor ();
}

void    captureMouse ( View * view )
{
	mouseOwner = view;
}

void    releaseMouse ()
{
	mouseOwner = NULL;
}


#pragma off (check_stack)
static void _loadds far mouseHandler ( int mask, int btn, int x, int y )
{
#pragma aux mouseHandler parm [EAX] [EBX] [ECX] [EDX]

	if ( mouseFreeze > 0 )
                return;

	__asm push es;                  // set ES = DS
	__asm push ds;
	__asm pop  es;

	if ( (mask & MOUSE_MOVE_MASK) && (mouseHidden < 1) && ( curCursor -> image -> isOk () ) )
	{
		Rect    saveClip ( screenSurface -> getClipRect () );
		Point	saveOrg  ( screenSurface -> getOrg () );
		int	saveRop = screenSurface -> getRop ();

		screenSurface -> setClipRect ( screenRect );
		screenSurface -> org.x = 0;
		screenSurface -> org.y = 0;

		saveRect.x1 = x;
		saveRect.y1 = y;
		saveRect.x2 = x + saveSurface -> width - 1;
		saveRect.y2 = y + saveSurface -> height - 1;

		copyRect.x1 = 0;
		copyRect.y1 = 0;
		copyRect.x2 = curCursor -> image -> width - 1;
		copyRect.y2 = curCursor -> image -> height - 1;

		saveSurface -> copy ( * screenSurface, copyRect, curMouseState.loc );

		curMouseState.loc.x = x;
		curMouseState.loc.y = y;

		screenSurface -> copy ( *saveSurface, saveRect, dstPt );
		curCursor     -> image -> copyTransp ( *screenSurface, copyRect, curMouseState.loc, curCursor -> transpColor );

		screenSurface -> setClipRect ( saveClip );
		screenSurface -> setOrg      ( saveOrg  );
		screenSurface -> setRop      ( saveRop  );
	}

	curMouseState.loc.x   = x;
	curMouseState.loc.y   = y;
	curMouseState.buttons = btn;

	if ( mask & MOUSE_MOVE_MASK )
		messageQueue.postMessage ( mouseOwner, WM_MOUSEMOVE );
	else
	if ( mask & MOUSE_LBUTTON_PRESS )
	{
		if ( curMouseState.loc == lastClickPoint && abs ( lastClickTime - getTimerTicks () ) < DOUBLE_CLICK_THRESHOLD )
		{
			if ( doubleClickFlag )
				messageQueue.postMessage ( mouseOwner, WM_TRIPLECLICK );
			else
				messageQueue.postMessage ( mouseOwner, WM_DBLCLICK );

			doubleClickFlag = TRUE;
		}
		else
		{
			messageQueue.postMessage ( mouseOwner, WM_LBUTTONDOWN );

			doubleClickFlag = FALSE;
		}

		lastClickPoint = curMouseState.loc;
		lastClickTime  = getTimerTicks ();
	}
	else
	if ( mask & MOUSE_LBUTTON_RELEASE )
		messageQueue.postMessage ( mouseOwner, WM_LBUTTONUP );
	else
	if ( mask & MOUSE_RBUTTON_PRESS )
		messageQueue.postMessage ( mouseOwner, WM_RBUTTONDOWN );
	else
	if ( mask & MOUSE_RBUTTON_RELEASE )
		messageQueue.postMessage ( mouseOwner, WM_RBUTTONUP );

	__asm pop es;
}
#pragma on (check_stack)

int initMouse ()
{
	Surface * arrowImage = loadBMPFromResource ( systemResource, "arrow" );

	arrowCursor = new CursorShape ( arrowImage, Point ( 0, 0 ), RGB ( 0, 0, 0xFF ) );
	saveSurface = new Surface ( 32, 32, screenSurface -> format );

	union   REGS  inRegs, outRegs;
	struct  SREGS segRegs;

	inRegs.w.ax = 0;

	int386 ( 0x33, &inRegs, &outRegs );

	if ( outRegs.w.ax != 0xFFFF )           // check for mouse presence
		return FALSE;

	curMouseState.loc.x = 0;
	curMouseState.loc.y = 0;
	hideRangeDefined    = 0;

	inRegs.w.ax = 4;                        // move mouse cursor to (0,0)
	inRegs.w.cx = 0;
	inRegs.w.dx = 0;

	int386 ( 0x33, &inRegs, &outRegs );

	setHorzMouseRange ( 0, screenRect.x2 );	// adjust mouse range
	setVertMouseRange ( 0, screenRect.y2 );

	setMouseShape   ( arrowCursor );        // set up arrow cursor
	showMouseCursor ();                     // and show it

	segread ( &segRegs );

	inRegs.w.ax  = 0x0C;                    // install mouse handler
	inRegs.w.cx  = 0x0F;
	inRegs.x.edx = FP_OFF ( mouseHandler );
	segRegs.es   = FP_SEG ( mouseHandler );

	int386x ( 0x33, &inRegs, &outRegs, &segRegs );

	return TRUE;
}

void doneMouse ()
{
	union REGS  inRegs, outRegs;

        hideMouseCursor ();                     // hide cursor

        inRegs.w.ax  = 0x0C;                    // and remove mouse handler
        inRegs.w.cx  = 0;

        int386 ( 0x33, &inRegs, &outRegs );

        delete arrowCursor;
        delete saveSurface;
}
