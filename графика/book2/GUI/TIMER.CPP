//
// Basic timer functions
// Author: Alex V. Boreskoff
//
#include        <dos.h>
#include        "array.h"
#include        "message.h"
#include        "timer.h"

struct  TimerStruct
{
        View  * view;
        int     id;
        int     ticksTilEvent;
        int     period;
};

static  Array timers ( 10, 10 );
static  void (__interrupt __far *oldTimerProc) ();

void    __interrupt __far timerProc ()
{
        oldTimerProc ();

        for ( register int i = 0; i < timers.getCount (); i++ )
        {
                TimerStruct * timer = (TimerStruct *)timers [i];

                if ( ( timer -> ticksTilEvent -= 55 ) <= 0 )
                {
                        timer -> ticksTilEvent = timer -> period;

                        messageQueue.postMessage ( timer -> view, WM_TIMER, timer -> id );
                }
        }
}

void    startPeriodicEvents ( View * view, int id, int delay, int period )
{
        TimerStruct * timer = new TimerStruct;

        timer -> view          = view;
        timer -> id            = id;
        timer -> ticksTilEvent = delay;
        timer -> period        = period;

        timers.insert ( timer );
}

void    stopPeriodicEvents  ( View * view, int id )
{
        for ( int i = 0; i < timers.getCount (); i++ )
        {
                TimerStruct * timer = (TimerStruct *)timers [i];

                if ( timer -> view == view && timer -> id == id )
                {
                        delete (TimerStruct *) timers.atRemove ( i );

                        return;
                }
        }
}

void    initTimer ()
{
        oldTimerProc = _dos_getvect ( 0x08 );
        _dos_setvect ( 0x08, timerProc );
}

void    doneTimer ()
{
        _dos_setvect ( 0x08, oldTimerProc );
}

