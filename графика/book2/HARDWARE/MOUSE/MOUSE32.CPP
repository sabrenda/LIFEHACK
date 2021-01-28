//
// This file provides mouse interface for DPMI32 for Watcom compiler
//

#include        <stdio.h>
#include        <dos.h>
#include        <i86.h>
#include        "mouse.h"

static  MouseHandler    curHandler = NULL;

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

int     resetMouse ()
{
        union REGS inRegs, outRegs;

        inRegs.w.ax = 0;

        int386 ( 0x33, &inRegs, &outRegs );

        return outRegs.w.ax == 0xFFFF;
}

void    showMouseCursor ()
{
        union REGS inRegs, outRegs;

        inRegs.w.ax = 1;

        int386 ( 0x33, &inRegs, &outRegs );
}

void    hideMouseCursor ()
{
        union REGS inRegs, outRegs;

        inRegs.w.ax = 2;

        int386 ( 0x33, &inRegs, &outRegs );
}

void    readMouseState ( MouseState& s )
{
        union REGS inRegs, outRegs;

	inRegs.w.ax = 3;

	int386 ( 0x33, &inRegs, &outRegs );

	s.loc.x   = outRegs.w.cx;
	s.loc.y   = outRegs.w.dx;
	s.buttons = outRegs.w.bx;
}

void    moveMouseCursor ( const Point p )
{
	union REGS inRegs, outRegs;

	inRegs.w.ax = 4;
	inRegs.w.cx = p.x;
	inRegs.w.dx = p.y;

	int386 ( 0x33, &inRegs, &outRegs );
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

void    setMouseShape ( const CursorShape& c )
{
        union  REGS  inRegs, outRegs;
        struct SREGS segRegs;

        segread ( &segRegs );

        inRegs.w.ax  = 9;
        inRegs.w.bx  = c.hotSpot.x;
        inRegs.w.cx  = c.hotSpot.y;
        inRegs.x.edx = FP_OFF ( c.andMask );
        segRegs.es   = FP_SEG ( c.andMask );

        int386x ( 0x33, &inRegs, &outRegs, &segRegs );
}

void    hideMouseCursor ( const Rect& r )
{
        union REGS  inRegs, outRegs;

        inRegs.w.ax = 0x10;
        inRegs.w.bx = r.x1;
        inRegs.w.cx = r.y1;
        inRegs.w.si = r.x2;
        inRegs.w.di = r.y2;     // ????

        int386 ( 0x33, &inRegs, &outRegs );
}

#pragma off (check_stack)
static void _loadds far mouseStub ( int mask, int btn, int x, int y )
{
#pragma aux mouseStub parm [EAX] [EBX] [ECX] [EDX]
        (*curHandler)(mask, btn, x, y );
}
#pragma on (check_stack)

void    setMouseHandler ( MouseHandler h, int mask )
{
        curHandler = h;

        union  REGS  inRegs, outRegs;
        struct SREGS segRegs;

        segread ( &segRegs );

        inRegs.w.ax  = 0x0C;
        inRegs.w.cx  = (short) mask;
        inRegs.x.edx = FP_OFF ( mouseStub );
        segRegs.es   = FP_SEG ( mouseStub );

        int386x ( 0x33, &inRegs, &outRegs, &segRegs );
}

void    removeMouseHandler ()
{
        union REGS  inRegs, outRegs;

        inRegs.w.ax  = 0x0C;
        inRegs.w.cx  = 0;

        int386 ( 0x33, &inRegs, &outRegs );

        curHandler   = NULL;
}
