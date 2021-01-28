//
// Basic timer functions
// Author: Alex V. Boreskoff
//
#ifndef __TIMER__
#define __TIMER__

class   View;

void    startPeriodicEvents ( View * view, int id, int delay, int period );
void    stopPeriodicEvents  ( View * view, int id );
void    initTimer ();
void    doneTimer ();

#endif
