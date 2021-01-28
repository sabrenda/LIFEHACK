#ifndef	__MOUSE__
#define	__MOUSE__

#include	"point.h"
#include	"rect.h"
					// mouse event flags
#define	MOUSE_MOVE_MASK		0x01
#define	MOUSE_LBUTTON_PRESS	0x02
#define	MOUSE_LBUTTON_RELEASE	0x04
#define	MOUSE_RBUTTON_PRESS	0x08
#define	MOUSE_RBUTTON_RELEASE	0x10
#define	MOUSE_MBUTTON_PRESS	0x20
#define	MOUSE_MBUTTON_RELEASE	0x40
#define	MOUSE_ALL_EVENTS	0x7F
					// button flags
#define	MK_LEFT			0x0001
#define	MK_RIGHT		0x0002
#define	MK_MIDDLE		0x0004

struct	MouseState
{
	Point	loc;
	int	buttons;
};

class	CursorShape
{
public:
	unsigned short andMask [16];
	unsigned short xorMask [16];
	Point	       hotSpot;

	CursorShape ( unsigned short *, unsigned short *, const Point& );
};

typedef	void	(cdecl far * MouseHandler)( int, int, int, int );

int	resetMouse         ();
void	showMouseCursor    ();
void	hideMouseCursor    ();
void	hideMouseCursor    ( const Rect& );
void	readMouseState     ( MouseState& );
void	moveMouseCursor    ( const Point );
void	setMouseHorzRange  ( int, int );
void	setmouseVertRange  ( int, int );
void	setMouseShape      ( const CursorShape& );
void	setMouseHandler    ( MouseHandler, int = MOUSE_ALL_EVENTS );
void	removeMouseHandler ();

#endif
