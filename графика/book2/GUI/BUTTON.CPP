//
// Button classes for GUI
// Author : Alex V. Boreskoff
//

#include	<dos.h>

#include	"win.h"
#include	"button.h"
#include	"store.h"
#include	"keys.h"

////////////////////////////////// PushButton /////////////////////////////////

void	PushButton :: performClick ()
{
	status |= BS_PRESSED;
	repaint ();
	delay   ( 100 );
	status &= ~BS_PRESSED;
	repaint ();
	sendMessage ( delegate, WM_COMMAND, 0, tag, this );
}

int	PushButton :: keyDown ( const Message& m )
{
	if ( isEnabled () && ( m.param == ' ' || m.param == CR ) )
	{
		performClick ();

		return TRUE;
	}

	return View :: keyDown ( m );
}

int	PushButton :: mouseDown ( const Message& m )
{
	if ( contains ( m.mouse.loc ) )
	{
		status |= BS_PRESSED | BS_TRACKING;
		repaint ();
		captureMouse ( this );
	}

	return TRUE;
}

int	PushButton :: mouseUp ( const Message& m )
{
	if ( !isEnabled () )
		return View :: mouseUp ( m );

	releaseMouse ();

	if ( status & BS_TRACKING )
	{
		status &= ~BS_TRACKING;

		if ( contains ( m.mouse.loc ) )
		{
			status &= ~BS_PRESSED;
			repaint ();
			sendMessage ( delegate, WM_COMMAND, 0, tag, this );

			return TRUE;
		}
	}

	return View :: mouseUp ( m );
}

int	PushButton :: mouseMove ( const Message& m )
{
	if ( !isEnabled () )
		return View :: mouseUp ( m );

	if ( ( m.mouse.buttons & MK_LEFT ) && status & BS_TRACKING )
		if ( contains ( m.mouse.loc ) )
		{
			if ( ! ( status & BS_PRESSED ) )
			{
				status |= BS_PRESSED;
				repaint ();
			}
		}
		else
			if ( status & BS_PRESSED )
			{
				status &= ~BS_PRESSED;
				repaint ();
			}

	return TRUE;
}

////////////////////////////////// TextButton //////////////////////////////

void	TextButton :: draw ( const Rect& r ) const
{
	int	w = width () - 1;
	int	h = height () - 1;
	int	tw, th;

	if ( status & BS_PRESSED )
	{
		screenSurface -> setColor ( LIGHTGRAY );
		screenSurface -> drawBar  ( 0, 0, w, h );
		screenSurface -> setColor ( BLACK );
		screenSurface -> drawLine ( 0, 0, w - 1, 0 );
		screenSurface -> drawLine ( w - 1, 0, w - 1, h - 1 );
		screenSurface -> drawLine ( w - 1, h - 1, 0, h - 1 );
		screenSurface -> drawLine ( 0, h - 1, 0, 0 );
		screenSurface -> setColor ( BLACK );

		sysFont -> size ( tw, th, text );
		screenSurface -> drawString ( 3 + ( ( w - tw ) >> 1 ), 1 + ( ( h - th ) >> 1 ), text );

	}
	else
	{
		if ( isFocused () )
		{
			screenSurface -> setColor ( BLACK );
			screenSurface -> drawFrame ( Rect ( 0, 0, w, h ) );
			screenSurface -> setColor ( LIGHTGRAY );
			screenSurface -> drawBar  ( 1, 1, w - 2, h - 2 );
			screenSurface -> setColor ( WHITE );
			screenSurface -> drawLine ( 1, 1, w-1, 1 );
			screenSurface -> drawLine ( 1, 1, 1, h - 1 );
			screenSurface -> setColor ( DARKGRAY );
			screenSurface -> drawLine ( 1, h - 1, w - 1, h - 1 );
			screenSurface -> drawLine ( w -1, 1, w - 1, h - 1);

		}
		else
		{
			screenSurface -> setColor ( LIGHTGRAY );
			screenSurface -> drawBar  ( 0, 0, w, h );
			screenSurface -> setColor ( WHITE );
			screenSurface -> drawLine ( 0, 0, w, 0 );
			screenSurface -> drawLine ( 0, 0, 0, h );
			screenSurface -> setColor ( DARKGRAY );
			screenSurface -> drawLine ( 0, h, w, h );
			screenSurface -> drawLine ( w, 0, w, h );
		}

		sysFont -> size ( tw, th, text );
		screenSurface -> drawString ( 2 + ( ( w - tw ) >> 1 ), ( ( h - th ) >> 1 ), text );
	}
}

////////////////////////////////// PicButton ///////////////////////////////

PicButton :: PicButton ( int x, int y, View * p, Surface * up, Surface * down, Surface * focused, int ident )
	: PushButton ( x, y, up -> width, up -> height, p, ident )
{
	face [0] = up;
	face [1] = down;
	face [2] = focused;
	status  |= WS_ENABLED | WS_REPAINTONFOCUS | WS_VISIBLE;
}

PicButton :: ~PicButton ()
{
	delete face [0];
	delete face [1];
	delete face [2];
}

int	PicButton :: put ( Store * s ) const
{
	if ( !View :: put ( s ) )
		return FALSE;

	face [0] -> put ( s );
	face [1] -> put ( s );
	face [2] -> put ( s );

	return TRUE;
}

int	PicButton :: get ( Store * s )
{
	if ( !View :: get ( s ) )
		return FALSE;

	face [0] = (Surface *) s -> getObject ();
	face [1] = (Surface *) s -> getObject ();
	face [2] = (Surface *) s -> getObject ();

	return TRUE;
}

void	PicButton :: draw ( const Rect& ) const
{
	if ( !isEnabled () )
		;//face [0] -> PutDisabled ( 0, 0 );
	else
	if ( status & BS_PRESSED )
		face [1] -> copy ( *screenSurface, Point ( 0, 0 ) );
	else
	if ( isFocused () )
		face [2] -> copy ( *screenSurface, Point ( 0, 0 ) );
	else
		face [0] -> copy ( *screenSurface, Point ( 0, 0 ) );
}

Object * loadPushButton ()
{
	return new PushButton;
}

Object * loadTextButton ()
{
	return new TextButton;
}

Object * loadPicButton ()
{
	return new PicButton;
}
