#include	<alloc.h>
#include	"Mouse.h"

#pragma inline

static	MouseHandler	curHandler = NULL;

CursorShape :: CursorShape ( unsigned short * aMask, unsigned short * xMask,
			     const Point& p ) : hotSpot ( p )
{
	for ( register int i = 0; i < 16; i++ )
	{
		andMask [i] = aMask [i];
		xorMask [i] = xMask [i];
	}
}

///////////////////////////////////////////////////////////////////////////

int	resetMouse ()
{
	asm {
		xor	ax, ax
		int	33h
	}

	return _AX == 0xFFFF;
}

void	showMouseCursor ()
{
	asm {
		mov	ax, 1
		int	33h
	}
}

void	hideMouseCursor ()
{
	asm {
		mov	ax, 2
		int	33h
	}
}

void	readMouseState ( MouseState& s )
{
	asm {
		mov	ax, 3
		int	33h
	}

#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr s
		mov	es:[di  ], cx
		mov	es:[di+2], dx
		mov	es:[di+4], bx
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr s
		mov	[di  ], cx
		mov	[di+2], dx
		mov	[di+4], bx
		pop	di
	}
#endif
}

void	moveMouseCursor ( const Point p )
{
	asm {
		mov	ax, 4
		mov	cx, p.x
		mov	dx, p.y
		int	33h
	}
}

void	setHorzMouseRange ( int xmin, int xmax )
{
	asm {
		mov	ax, 7
		mov	cx, xmin
		mov	dx, xmax
		int	33h
	}
}

void	setVertMouseRange ( int ymin, int ymax )
{
	asm {
		mov	ax, 8
		mov	cx, ymin
		mov	dx, ymax
		int	33h
	}
}

void	setMouseShape ( const CursorShape& c )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr c
		mov	bx, es:[di+64]
		mov	cx, es:[di+66]
		mov	dx, di
		mov	ax, 9
		int	33h
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr c
		mov	bx, [di+64]
		mov	cx, [di+66]
		mov	dx, di
		mov	ax, 9
		int	33h
		pop	di
	}
#endif
}

void    hideMouseCursor ( const Rect& r )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	si
		push	di
		les	di, dword ptr r
		mov	ax, 10h
		mov	cx, es:[di]
		mov	dx, es:[di+2]
		mov	si, es:[di+4]
		mov	di, es:[di+6]
		int	33h
		pop	di
		pop	si
		pop	es
	}
#else
	asm {
		push	si
		push	di
		mov	di, word ptr r
		mov	ax, 10h
		mov	cx, [di]
		mov	dx, [di+2]
		mov	si, [di+4]
		mov	di, [di+6]
		int	33h
		pop	di
		pop	si
	}
#endif
}

static void far mouseStub ()
{
	asm {
		push	ds	// preserve ds
		push	ax	// preserve ax
		mov	ax, seg curHandler
		mov	ds, ax
		pop	ax	// restore ax
		push	dx	// y
		push	cx	// x
		push	bx	// button state
		push	ax	// event mask
		call	curHandler
		add	sp, 8	// clear stack
		pop	ds
	}
}

void	setMouseHandler ( MouseHandler h, int mask )
{
	void	far * p = mouseStub;

	curHandler = h;

	asm {
		push	es
		mov	ax, 0Ch
		mov	cx, mask
		les	dx, p
		int	33h
		pop	es
	}
}

void	removeMouseHandler ()
{
	curHandler = NULL;

	asm {
		mov	ax, 0Ch
		mov	cx, 0
		int	33h
	}
}
