//
// Standart window & dialog window classes, desktop class
// Author: Alex V. Boreskoff
//
#include        "objdict.h"
#include        "store.h"
#include        "keys.h"
#include        "win.h"
#include        "font.h"
#include        "loader.h"
#include        "message.h"
#include        "video.h"
#include	"resfile.h"

Object * loadPushButton ();
Object * loadTextButton ();
Object * loadPicButton  ();
Object * loadRadioGroup ();
Object * loadStaticText ();
Object * loadStaticPic  ();

////////////////////////// Global variables  //////////////////////////////

Window        * activeWindow = NULL;
Font          * sysFont;
Font          * sysFixedFont;
Surface       * closeBox [2];
Surface       * sysCheckBox [3];
Surface       * sysRadioButton [3];
Surface       * sysArrows [8];
Surface       * marker;
Surface       * ind [2];

///////////////////////////// DeskTop methods /////////////////////////////

DeskTop :: DeskTop () : View ( 0, 0, screenSurface -> width, screenSurface -> height )
{
	running = TRUE;

	strcpy ( text, "DeskTop" );
	show   ();
}

void    DeskTop :: run ()
{
        Message m;

        while ( running )
        {
                autoreleasePool.deleteAll ();

                if ( messageQueue.getMessage ( m ) )
                        handle ( m );
                else
                        idle ();
        }
}

void    DeskTop :: draw ( const Rect& r ) const
{
        screenSurface -> setColor ( DARKGRAY );
        screenSurface -> drawBar  ( 0, 0, area.x2 - area.x1, area.y2 - area.y1 );
}

int     DeskTop :: handle ( const Message& m )
{
        if ( (m.code == WM_KEYDOWN || m.code == WM_KEYUP) && m.param == AltX )
        {
                running = FALSE;

                return TRUE;                            // processed
        }

        if ( m.isMouseMessage () )
        {
                Message tmp = m;

                if ( m.view != NULL )
                {
                        tmp.view -> screen2Local ( tmp.mouse.loc );

                        return tmp.view -> handle ( tmp );
                }

                View * view = findView ( tmp.mouse.loc );

                if ( view != this && view != NULL )
                {
                        view -> screen2Local ( tmp.mouse.loc );

                        if ( view -> isEnabled () && tmp.code != WM_MOUSEMOVE )
                                setFocus ( view );

                        return view -> handle ( tmp );
                }

                return FALSE;
        }
        else
        if ( m.isKeyboardMessage () && focusedView != this && focusedView != NULL )
                return focusedView -> handle ( m );

        return View :: handle ( m );
}

int     DeskTop :: command ( const Message& m )
{
        if ( m.param == IDQUIT )
                running = FALSE;

        return TRUE;
}

/////////////////////////////  Window  methods //////////////////////////////

void    Window :: draw ( const Rect& r ) const
{
        Rect    clientRect;

        getClientRect ( clientRect );
        local2Screen  ( clientRect );

        drawFrame ();

        screenSurface -> clipRect &= clientRect;
        screenSurface -> setColor ( LIGHTGRAY );
        screenSurface -> drawBar  ( 0, 0, width (), height () );
}

void    Window :: drawFrame () const
{
        int     w = width  () - 1;
        int     h = height () - 1;
        Rect    r ( 0, 0, w, h );

        screenSurface -> setColor ( BLACK );
        screenSurface -> drawFrame ( r );
        screenSurface -> drawLine ( 0, CAPTION_SIZE + 1,  w, CAPTION_SIZE + 1 );

        if ( isActive () || isFloating () )
                screenSurface -> setColor ( LIGHTGRAY );
        else
                screenSurface -> setColor ( WHITE );

        screenSurface -> drawLine ( 1, 1, w - 1, 1 );
        screenSurface -> drawLine ( 1, 1, 1,     CAPTION_SIZE );

        screenSurface -> setColor ( DARKGRAY );
        screenSurface -> drawLine ( 2,     CAPTION_SIZE, w - 1, CAPTION_SIZE );
        screenSurface -> drawLine ( w - 1, CAPTION_SIZE, w - 1, 2 );

        if ( isActive () || isFloating () )
                screenSurface -> setColor ( BLACK );
        else
                screenSurface -> setColor ( LIGHTGRAY );

        screenSurface -> drawBar ( 2, 2, w - 2, CAPTION_SIZE - 1 );

        if ( isActive () || isFloating () )
                screenSurface -> setColor ( WHITE );
        else
                screenSurface -> setColor ( BLACK );

        screenSurface -> setFont ( sysFont );
        screenSurface -> drawString ( 20, (CAPTION_SIZE - sysFont -> height)/2, text );

        if ( style & WS_HASCLOSEBOX )
                closeBox [0] -> copy ( *screenSurface, Point ( w - 17, 4 ) );

        if ( style & WS_RESIZEABLE )
        {
                screenSurface -> setColor ( LIGHTGRAY );
                screenSurface -> drawBar ( 1, h - 9, w - 2, h - 1 );

                screenSurface -> setColor ( DARKGRAY );
                screenSurface -> drawLine ( 1,      h - 9, w - 1,  h - 9 );
                screenSurface -> drawLine ( 16,     h - 8, 16,     h - 1 );
                screenSurface -> drawLine ( w - 16, h - 8, w - 16, h - 1 );

                screenSurface -> setColor ( WHITE );
                screenSurface -> drawLine ( 1,      h - 8, w - 1,  h - 8 );
                screenSurface -> drawLine ( 17,     h - 8, 17,     h - 1 );
                screenSurface -> drawLine ( w - 15, h - 8, w - 15, h - 1 );
        }
}

int     Window :: receiveFocus ( const Message& m )
{
        if ( activeWindow != this )
                activate ();

        return View :: receiveFocus ( m );
}

int     Window :: command ( const Message& m )
{
        if ( m.subCode == VN_FOCUS && m.param ) // child received focus
                if ( activeWindow != this )
                        activate ();

        return View :: command ( m );
}

int     Window :: mouseDown ( const Message& m )
{
        switch ( hitTest ( m.mouse.loc ) )
        {
                case HT_CLOSE:
                        trackClose ();
                        break;

                case HT_CAPTION:
                        if ( style & WS_MOVEABLE )
                                trackDrag ( m.mouse.loc );

                        break;

                case HT_RESIZE:
                        trackResize ( 3, m.mouse.loc );
                        break;

                case HT_RESIZEVER:
                        trackResize ( 2, m.mouse.loc );
                        break;

                default:
                        return View :: mouseDown ( m );
        }

        return TRUE;
}

Rect    Window :: getClientRect ( Rect& r ) const
{
        r.x1 = 1;
        r.y1 = CAPTION_SIZE + 2;
        r.x2 = area.x2 - area.x1 - 1;
        r.y2 = area.y2 - area.y1 - 1;

        if ( style & WS_RESIZEABLE )
                r.y2 -= 9;

        return r;
}

void    Window :: getMinMaxSize ( Point& minSize, Point& maxSize ) const
{
        minSize.x = 100;
        minSize.y = CAPTION_SIZE + 12;
        maxSize.x = screenSurface -> width;
        maxSize.y = screenSurface -> height;
}

int     Window :: hitTest ( const Point& p ) const
{
        if ( style & WS_HASCLOSEBOX )
                if ( p.x >= width () - 18 && p.x < width () - 4 && p.y >= 4 && p.y < 18 )
                        return HT_CLOSE;

        if ( p.y >= 0 && p.y <= CAPTION_SIZE )
                return HT_CAPTION;

        if ( style & WS_RESIZEABLE )
                if ( p.y >= area.y2 - area.y1 - 7 && p.y < area.y2 - area.y1 )
                        return p.x >= area.x2 - area.x1 - 15 ? HT_RESIZE : HT_RESIZEVER;

        return HT_CLIENT;
}

void    Window :: trackClose ()
{
        Message m;
        int     hitCode;

        screenSurface -> setClipRect ( screenRect );
        screenSurface -> org.x = 0;
        screenSurface -> org.y = 0;

        local2Screen ( screenSurface -> org );

        hideMouseCursor ();
        closeBox [1] -> copy ( *screenSurface, Point ( width () - 18, 4 ) );
        showMouseCursor ();

        do
        {
                if ( !messageQueue.getMessage ( m ) )
                        continue;

                if ( !m.isMouseMessage () )
                        continue;

                screen2Local ( m.mouse.loc );
                if ( ( hitCode = hitTest ( m.mouse.loc ) ) != HT_CLOSE )
                        break;

        } while ( m.code != WM_LBUTTONUP );

        hideMouseCursor ();
        closeBox [0] -> copy ( * screenSurface, Point ( width () - 18, 4 ) );
        showMouseCursor ();

        if ( hitCode == HT_CLOSE )
                close ( m );
}

void    Window :: trackDrag ( Point mouseLoc )
{
        Message m;
        int     rop = screenSurface -> getRop ();
        Rect    w ( getScreenRect () );

        screenSurface -> setRop ( RO_XOR );
        screenSurface -> setClipRect ( screenRect );

        screenSurface -> org.x = 0;
        screenSurface -> org.y = 0;

        screenSurface -> setColor ( WHITE );

        local2Screen    ( mouseLoc );
        hideMouseCursor ();

        screenSurface -> drawFrame ( w );                       // show frame

        for ( ; ; )
        {
                if ( !messageQueue.getMessage ( m ) )
                        continue;

                if ( m.code == WM_MOUSEMOVE )
                {
                        screenSurface -> drawFrame ( w );       // erase frame
                        w.move ( m.mouse.loc.x - mouseLoc.x, m.mouse.loc.y - mouseLoc.y );
                        screenSurface -> drawFrame ( w );       // draw it

                        mouseLoc = m.mouse.loc;
                }
                else
                if ( m.code == WM_LBUTTONUP )
                        break;
        }

        screenSurface -> drawFrame ( w );
        screenSurface -> setRop    ( rop );

        if ( parent != NULL )
                parent -> screen2Local ( w );

        setFrame ( w );
        showMouseCursor ();
}

void    Window :: trackResize ( int mode, Point mouseLoc )
{
        Message m;
        int     rop = screenSurface -> getRop ();
        Rect    w ( getScreenRect () );
        Point   minSize, maxSize;

        screenSurface -> org.x = 0;
        screenSurface -> org.y = 0;
        screenSurface -> setClipRect ( screenRect );
        screenSurface -> setColor ( WHITE );
        screenSurface -> setRop ( RO_XOR );

        getMinMaxSize   ( minSize, maxSize );
        local2Screen    ( mouseLoc );
        hideMouseCursor ();

        screenSurface -> drawFrame  ( w );                      // show border

        for ( ; ; )
        {
                if ( !messageQueue.getMessage ( m ) )
                        continue;

                if ( m.code == WM_MOUSEMOVE )
                {
                        screenSurface -> drawFrame ( w );       // hide frame

                        if ( mode & 0x0001 )
                                w.x2 += m.mouse.loc.x - mouseLoc.x;

                        if ( mode & 0x0002 )
                                w.y2 += m.mouse.loc.y - mouseLoc.y;

                        if ( w.x2 - w.x1 + 1 < minSize.x )
                                w.x2 = w.x1 + minSize.x - 1;
                        else
                        if ( w.x2 - w.x1 + 1 > maxSize.x )
                                w.x2 = w.x1 + maxSize.x - 1;

                        if ( w.y2 - w.y1 + 1 < minSize.y )
                                w.y2 = w.y1 + minSize.y - 1;
                        else
                        if ( w.y2 - w.y1 + 1 > maxSize.y )
                                w.y2 = w.y1 + maxSize.y - 1;

                        screenSurface -> drawFrame ( w );       // show frame

                        mouseLoc = m.mouse.loc;
                }
                else
                if ( m.code == WM_LBUTTONUP )
                        break;
        }

        screenSurface -> drawFrame ( w );                       // hide frame
        screenSurface -> setRop    ( rop );

        if ( parent != NULL )
                parent -> screen2Local ( w );

        setFrame ( w );
        showMouseCursor ();
}

void    Window :: activate ()
{
        if ( activeWindow == this )
                return;

        if ( activeWindow != NULL )
                activeWindow -> deactivate ();

        activeWindow = this;
        status      |= WS_ACTIVE;

        setZOrder ( NULL );                     // bring window to front & repaint
}

void    Window :: deactivate ()
{
        status &= ~WS_ACTIVE;
        activeWindow = NULL;

        repaint ();
}


/////////////////////////// DialogWindow methods ////////////////////////////

DialogWindow :: DialogWindow ( int x, int y, int w, int h, char * t, View * p ) : Window ( x, y, w, h, t, p )
{
        dialogActive = 0;
        dialogCode   = 0;
        defaultID    = IDOK;
}

int     DialogWindow :: put ( Store * s ) const
{
        if ( !View :: put ( s ) )
                return FALSE;

        s -> putInt ( defaultID );

        return TRUE;
}

int     DialogWindow :: get ( Store * s )
{
        if ( !View :: get ( s ) )
                return FALSE;

        defaultID = s -> getInt ();

        return TRUE;
}

int     DialogWindow :: execute ()
{
        Message m;

        saveFocus    = focusedView;
        dialogActive = TRUE;
        dialogCode   = 0;

        prepDialog ();          // prepare dialog
                                // prepare all controls
        for ( View * v = child; v != NULL; v = v -> prev )
                v -> init ();

        show ();

        if ( ( v = getFirstTab () ) != NULL )
                setFocus ( v );
        else
                setFocus ( this );

        initDialog ();

        while ( dialogActive )
        {
                autoreleasePool.deleteAll ();

                if ( messageQueue.getMessage ( m ) )
                        handle     ( m );
                else
                        idle ();
        }

        hide     ();
        setFocus ( saveFocus );

        return dialogCode;
}


int     DialogWindow :: handle ( const Message& m )
{
        if ( m.isMouseMessage () )
        {
                Point   p ( m.mouse.loc );

                screen2Local ( p );

                if ( !contains ( p ) )
                {
                        if ( m.code == WM_LBUTTONDOWN || m.code == WM_RBUTTONDOWN )
                                messageBeep ();

                        return 0;       // we're done with it
                }

                Message tmp = m;

                if ( m.view != NULL )
                {
                        tmp.view -> screen2Local ( tmp.mouse.loc );

                        return tmp.view -> handle ( tmp );
                }

                View * view = findView ( tmp.mouse.loc );

                if ( view != this && view != NULL )
                {
                        view -> screen2Local ( tmp.mouse.loc );

                        if ( view -> isEnabled () && tmp.code != WM_MOUSEMOVE )
                                setFocus ( view );

                        return view -> handle ( tmp );
                }
                else
                if ( view == this )
                {
                        screen2Local ( tmp.mouse.loc );

                        return View :: handle ( tmp );
                }
        }
        else
        if ( m.isKeyboardMessage () && focusedView != this && focusedView != NULL )
                return focusedView -> handle ( m );

        return View :: handle ( m );
}

int     DialogWindow :: keyDown ( const Message& m )
{
        // if this message comes here it means no child has processed it
        // so we can process it

        switch ( m.param )
        {
                case CR:
                case CtrlEnter:
                        if ( defaultID != 0 )
                        {
                                sendMessage ( this, WM_COMMAND, 0, defaultID );
                                return 0;
                        }
                        break;

                case ESC:
                        sendMessage ( this, WM_COMMAND, 0, IDCANCEL );
                        return 0;

                case TAB:
                        setFocus ( nextTab ( focusedView ) );
                        return 0;

                case SHIFTTAB:
                        setFocus ( prevTab ( focusedView ) );
                        return 0;

                case DOWN:
                        moveFocusVert ( 1 );
                        return 0;

                case UP:
                        moveFocusVert ( -1 );
                        return 0;

                case LEFT:
                        moveFocusHorz ( -1 );
                        return 0;

                case RIGHT:
                        moveFocusHorz ( 1 );
                        return 0;
        }

        return View :: keyDown ( m );
}

int     DialogWindow :: receiveFocus ( const Message& m )
{
        View *  v = getFirstTab ();

        if ( v != NULL )
                setFocus ( v );

        return Window :: receiveFocus ( m );
}

int     DialogWindow :: command ( const Message& m )
{
        if ( m.param == IDOK )
                endDialog ( IDOK );
        else
        if ( m.param == IDCANCEL )
                endDialog ( IDCANCEL );

        return View :: command ( m );
}

int     DialogWindow :: close ( const Message& m )
{
        endDialog ( IDCANCEL );

        return View :: close ( m );
}

View *  DialogWindow :: prevTab ( View * v )
{
        do
        {
                if ( ( v = v -> prev ) == NULL )
                        v = child;
        } while ( ! ( v -> isVisible () && v -> isEnabled () ) );

        return v;
}

View *  DialogWindow :: nextTab ( View * v )
{
        do
        {
                if ( ( v = v -> next ) == NULL )
                {
                        for ( v = child; v -> prev != NULL; v = v -> prev )
                                        ;
                }
        } while ( ! ( v -> isVisible () && v -> isEnabled () ) );

        return v;
}

void    DialogWindow :: moveFocusVert ( int dir )
{
        int     d, dist;
                                // find 1st enabled item above Focus
        for ( View * best = child; best != NULL; best = best -> prev )
                if ( ( best -> area.y1 - focusedView -> area.y1 ) * dir > 0 && best -> isEnabled () && best -> isVisible () )
                        break;

        if ( best == NULL )
                return;
                                // find uppermost enabled item below Focus
        for ( View * c = best -> prev; c != NULL; c = c -> prev )
                if ( ( c -> area.y1 - focusedView -> area.y1 ) * dir > 0 && ( c -> area.y1 - best -> area.y1 ) * dir < 0 && c -> isEnabled () && c -> isVisible () )
                        best = c;

                                // find closest enabled item
        dist = getDistance ( focusedView, best );

        for ( c = child; c != NULL; c = c -> prev )
                if ( c -> area.y1 == best -> area.y1 && c -> isEnabled () && c -> isVisible () )
                        if ( ( d = getDistance ( focusedView, c ) ) >= dist )
                        {
                                dist = d;
                                best = c;
                        }

        setFocus ( best );
}

void    DialogWindow :: moveFocusHorz ( int dir )
{
        int     x1 = focusedView -> area.x1;
        int     y1 = focusedView -> area.y1;

        for ( View * best = child; best != NULL; best = best -> prev )
                if ( best -> area.y1 == y1 && ( best -> area.x1 - x1 ) * dir > 0 && best -> isEnabled () && best -> isVisible () )
                        break;

        if ( best == NULL )
                return;

        for ( View * c = best -> prev; c != NULL; c = c -> prev )
                if ( c -> area.y1 == y1 && ( c -> area.x1 - best  -> area.x1 ) * dir < 0 &&
                     ( c -> area.x1 - x1) * dir > 0 && c -> isEnabled () && c -> isVisible () )
                        best = c;

        setFocus ( best );
}

int     DialogWindow :: getDistance ( View * c1, View * c2 )
{
        return min ( c1 -> area.x2, c2 -> area.x2 ) - max ( c1 -> area.x1, c2 -> area.x1 );
}

///////////////////////////////////////////////////////////////////////////

Object * loadDeskTop ()
{
	return new DeskTop;
}

Object * loadWindow ()
{
	return new Window;
}

Object * loadDialogWindow ()
{
	return new DialogWindow;
}

void    initWindows ()
{
	sysFont            = loadFontFromResource ( systemResource, "SSERIF8" );
	sysFixedFont       = loadFontFromResource ( systemResource, "COUR8" );
	closeBox [0]       = loadBMPFromResource  ( systemResource, "SYS0" );
	closeBox [1]       = loadBMPFromResource  ( systemResource, "SYS1" );
	marker             = loadBMPFromResource  ( systemResource, "MARKER" );
	sysCheckBox [0]    = loadBMPFromResource  ( systemResource, "CHECK0" );
	sysCheckBox [1]    = loadBMPFromResource  ( systemResource, "CHECK1" );
	sysCheckBox [2]    = loadBMPFromResource  ( systemResource, "CHECK2" );
	sysRadioButton [0] = loadBMPFromResource  ( systemResource, "RADIO0" );
	sysRadioButton [1] = loadBMPFromResource  ( systemResource, "RADIO1" );
	sysArrows [0]      = loadBMPFromResource  ( systemResource, "UP0" );
	sysArrows [1]      = loadBMPFromResource  ( systemResource, "DOWN0" );
	sysArrows [2]      = loadBMPFromResource  ( systemResource, "LEFT0" );
	sysArrows [3]      = loadBMPFromResource  ( systemResource, "RIGHT0" );
	sysArrows [4]      = loadBMPFromResource  ( systemResource, "UP1" );
	sysArrows [5]      = loadBMPFromResource  ( systemResource, "DOWN1" );
	sysArrows [6]      = loadBMPFromResource  ( systemResource, "LEFT1" );
	sysArrows [7]      = loadBMPFromResource  ( systemResource, "RIGHT1" );
	ind [0]            = loadBMPFromResource  ( systemResource, "IND0" );
	ind [1]            = loadBMPFromResource  ( systemResource, "IND1" );

	registerClass ( "Surface",      loadSurface );
	registerClass ( "View",         loadView );
	registerClass ( "PushButton",   loadPushButton );
	registerClass ( "TextButton",   loadTextButton );
	registerClass ( "PicButton",    loadPicButton );
	registerClass ( "StaticText",   loadStaticText );
	registerClass ( "StaticPic",    loadStaticPic );
	registerClass ( "RadioGroup",   loadRadioGroup );
	registerClass ( "DeskTop",      loadDeskTop );
	registerClass ( "Window",       loadWindow );
	registerClass ( "DialogWindow", loadDialogWindow );
}

void    doneWindows ()
{
	delete sysFont;
	delete sysFixedFont;

	delete closeBox [0];
	delete closeBox [1];
	delete marker;
	delete sysCheckBox [0];
	delete sysCheckBox [1];
	delete sysCheckBox [2];
	delete sysRadioButton [0];
	delete sysRadioButton [1];
	delete sysArrows [0];
	delete sysArrows [1];
	delete sysArrows [2];
	delete sysArrows [3];
	delete sysArrows [4];
	delete sysArrows [5];
	delete sysArrows [6];
	delete sysArrows [7];
	delete ind [0];
	delete ind [1];
}

