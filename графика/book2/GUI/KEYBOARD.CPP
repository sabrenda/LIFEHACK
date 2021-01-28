//
// Basic keyboard functions for GUI
// Author: Alex V. Boreskoff
//
#include        <bios.h>
#include        <dos.h>
#include        <conio.h>
#include        "message.h"

static  void (__interrupt __far *oldKbdProc) ();

void    __interrupt __far kbdProc ()
{
	int     code     = inp ( 0x60 );
	int     scanCode = code & 0x7F;

	oldKbdProc ();

	if ( _bios_keybrd ( _KEYBRD_READY ) )
		scanCode = _bios_keybrd ( _KEYBRD_READ );

	if ( code & 0x80 )
		messageQueue.postMessage ( NULL, WM_KEYUP, 0, scanCode );
	else
		messageQueue.postMessage ( NULL, WM_KEYDOWN, 0, scanCode );
}

void    initKeyboard ()
{
	oldKbdProc = _dos_getvect ( 0x09 );
	_dos_setvect ( 0x09, kbdProc );
}

void    doneKeyboard ()
{
	_dos_setvect ( 0x09, oldKbdProc );
}

