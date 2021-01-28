//
// class MessageQueue realization & other misc. functions
// Author: Alex V. Boreskoff
//
#include        <dos.h>
#include        <stdarg.h>
#include        <stdio.h>

#include        "message.h"
#include        "view.h"

MessageQueue    messageQueue ( 100 );

//////////////////////// MessageQueue methods //////////////////////

#pragma off (check_stack)
void    MessageQueue :: postMessage ( View * view, int code, int subCode, int param, void * data )
{
	__asm cli;                              // lock queue
						// avoid multiple WM_MOUSEMOVE and WM_TIMER messages
	if ( !isEmpty () && ( code == WM_MOUSEMOVE || code == WM_TIMER ) )
	{
		int     prevPutPtr = putPtr - 1;

		if ( prevPutPtr < 0 )
			prevPutPtr = size - 1;

		if ( queue [prevPutPtr].code == code )
			putPtr = prevPutPtr;
	}

	queue [putPtr].time     = getTimerTicks ();
	queue [putPtr].kbdFlags = getKbdFlags ();
	queue [putPtr].view     = view;
	queue [putPtr].code     = code;
	queue [putPtr].subCode  = subCode;
	queue [putPtr].param    = param;
	queue [putPtr].data     = data;

	readMouseState ( queue [putPtr].mouse );

	if ( ++putPtr >= size )                 // wrap pointer
	{
		putPtr = 0;

		if ( getPtr == 0 )              // check for overflow
		{
			__asm sti;

			return;
		}
	}


	__asm sti;
}
#pragma on (check_stack)

int     MessageQueue :: getMessage ( Message& m )
{
	if ( isEmpty () )
		return FALSE;

	__asm cli;
	m = queue [getPtr++];

	if ( getPtr >= size )
		getPtr = 0;

	__asm sti;

	return TRUE;
}

int     sendMessage ( View * view, int code, int subCode, int param, void * data )
{
	Message m;

	if ( view == NULL )
		return 0;

	m.view     = view;
	m.code     = code;
	m.subCode  = subCode;
	m.param    = param;
	m.data     = data;
	m.time     = getTimerTicks ();
	m.kbdFlags = getKbdFlags ();

	readMouseState ( m.mouse );

	return view -> handle ( m );
}

int     getTimerTicks ()
{
	return * (int *) 0x46C;
}

int     getKbdFlags ()
{
	return * (short *) 0x417;
}

void    messageBeep ()
{
	sound ( 1000 );
	delay ( 125 );
	nosound ();
	delay ( 30 );
	sound ( 1000 );
	delay ( 125 );
	nosound ();
}

void    log ( char * msg, ... )
{
	va_list argptr;
	char    buf [1024];

	va_start ( argptr, msg );
	vsprintf ( buf, msg, argptr );
	va_end   ( argptr );

	FILE * logFile = fopen ( "log", "a" );

	fprintf ( logFile, "%s\n", buf );
	fflush  ( logFile );
	fclose  ( logFile );
}
