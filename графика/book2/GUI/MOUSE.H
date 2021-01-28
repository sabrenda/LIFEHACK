//
// Mouse support for VESA-modes
// Author: Alex V. Boreskoff
//
#ifndef __MOUSE__
#define __MOUSE__

#include        "point.h"
#include        "rect.h"
#include        "surface.h"
#include        "video.h"
                                        // mouse event flags
#define MOUSE_MOVE_MASK         0x01
#define MOUSE_LBUTTON_PRESS     0x02
#define MOUSE_LBUTTON_RELEASE   0x04
#define MOUSE_RBUTTON_PRESS     0x08
#define MOUSE_RBUTTON_RELEASE   0x10
#define MOUSE_MBUTTON_PRESS     0x20
#define MOUSE_MBUTTON_RELEASE   0x40
#define MOUSE_ALL_EVENTS        0x7F
                                        // button flags
#define MK_LEFT                 0x0001
#define MK_RIGHT                0x0002
#define MK_MIDDLE               0x0004

#define DOUBLE_CLICK_THRESHOLD  8       // time delta for two clicks
					// to become a double click
class   View;

struct  MouseState
{
        Point   loc;
        int     buttons;
};

class   CursorShape
{
public:
        Point     hotSpot;
        Surface * image;
        int       transpColor;

        CursorShape ()
        {
                image = NULL;
        }

        CursorShape ( Surface * img, const Point& hSpot, const RGB& trColor )
        {
                image       = img;
                hotSpot     = hSpot;
                transpColor = screenSurface -> closestColor ( trColor );
        }

        ~CursorShape ()
        {
                delete image;
        }
};

int     initMouse       ();
void    doneMouse       ();
void    showMouseCursor ();
void    hideMouseCursor ();
void    hideMouseCursor ( const Rect& );
void    readMouseState  ( MouseState& );
void    moveMouseCursor ( const Point );
void    setMouseShape   ( CursorShape * );
void    captureMouse    ( View * );
void    releaseMouse    ();

extern CursorShape * arrowCursor;
extern CursorShape * waitCursor;
#endif
