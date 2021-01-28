//
// Static text class for GUI
// Author: Alex V. Boreskoff
//

#include	<string.h>
#include	"static.h"
#include	"store.h"

////////////////////////////// StaticText /////////////////////////////////

StaticText :: StaticText ( int x, int y, int w, int h, View * p, char * t, int tc, int bc, char br, Font * fn, int del ) :
		View ( x, y, w, h, p )
{
	strcpy ( text, t );

	backColor    = bc;
	textColor    = tc;
	borderType   = br;
	fnt          = fn;
	shouldDelete = del;
	status      |= WS_VISIBLE;
}

int	StaticText :: put ( Store * s ) const
{
	if ( !View :: put ( s ) )
		return FALSE;

	s -> putInt ( textColor );
	s -> putInt ( backColor );
	s -> putInt ( borderType );

	return TRUE;
}

int	StaticText :: get ( Store * s )
{
	if ( !View :: get ( s ) )
		return FALSE;

	textColor  = s -> getInt ();
	backColor  = s -> getInt ();
	borderType = s -> getInt ();

	return TRUE;
}

void	StaticText :: draw ( const Rect& r ) const
{
	int	w = area.x2 - area.x1;
	int	h = area.y2 - area.y1;

	hideMouseCursor ();

	if ( borderType == SS_GRAY )
	{
		screenSurface -> setColor ( LIGHTGRAY );
		screenSurface -> drawLine ( 0, 0, w, 0 );
		screenSurface -> drawLine ( 0, 0, 0, h );
		screenSurface -> drawLine ( 0, h, w, h );
		screenSurface -> drawLine ( w, 0, w, h );
		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar  ( 1, 1, w - 1, h - 1 );
	}
	else
	if ( borderType == SS_BLACK )
	{
		screenSurface -> setColor ( BLACK );
		screenSurface -> drawLine ( 0, 0, w, 0 );
		screenSurface -> drawLine ( 0, 0, 0, h );
		screenSurface -> drawLine ( 0, h, w, h );
		screenSurface -> drawLine ( w, 0, w, h );
		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar ( 1, 1, w - 1, h - 1 );
	}
	else
	if ( borderType == SS_3D )
	{
		screenSurface -> setColor ( WHITE );
		screenSurface -> drawLine ( 0, h, w, h );
		screenSurface -> drawLine ( w, 0, w, h );

		screenSurface -> setColor ( LIGHTGRAY );
		screenSurface -> drawLine ( 1,     h - 1, w - 1, h - 1 );
		screenSurface -> drawLine ( w - 1, 1,     w - 1, h - 1 );

		screenSurface -> setColor ( DARKGRAY );
		screenSurface -> drawLine ( 0, 0, w - 1, 0 );
		screenSurface -> drawLine ( 0, 0, 0,     h - 1 );

		if ( backColor == DARKGRAY )
			screenSurface -> setColor ( BLACK );

		screenSurface -> drawLine ( 1, 1, w - 2, 1 );
		screenSurface -> drawLine ( 1, 1, 1,     h - 2 );

		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar ( 2, 2, w - 2, h - 2 );
	}
	else
	if ( borderType == SS_DOWN )
	{
		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar  ( 1, 1, w - 1, h - 1 );
		screenSurface -> setColor ( DARKGRAY );
		screenSurface -> drawLine ( 0, 0, w,     0 );
		screenSurface -> drawLine ( 0, 0, 0,     h );

		screenSurface -> setColor ( WHITE );
		screenSurface -> drawLine ( w, h, 1, h );
		screenSurface -> drawLine ( w, h, w, 1 );
	}
	else
	if ( borderType == SS_UP )
	{
		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar  ( 1, 1, w - 1, h - 1 );
		screenSurface -> setColor ( WHITE );
		screenSurface -> drawLine ( 0, 0, w, 0 );
		screenSurface -> drawLine ( 0, 0, 0, h );
		screenSurface -> setColor ( textColor );
		screenSurface -> drawLine ( w, h, 1, h );
		screenSurface -> drawLine ( w, h, w, 1 );
	}
	else
	if ( borderType == SS_FRAMED )
	{
		Rect	r ( 0, 0, w - 1, h - 1 );

		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar  ( 0, 0, w, h );
		screenSurface -> setColor ( DARKGRAY );
		screenSurface -> drawFrame ( r );
		r.move ( 1, 1 );
		screenSurface -> setColor ( WHITE );
		screenSurface -> drawFrame ( r );
	}
	else
	if ( borderType == SS_FRAMERAISED )
	{
		Rect	r ( 1, 1, w, h );

		screenSurface -> setColor ( backColor );
		screenSurface -> drawBar ( 0, 0, w, h );
		screenSurface -> setColor ( DARKGRAY );
		screenSurface -> drawFrame ( r );
		r.move ( -1, -1 );
		screenSurface -> setColor ( WHITE );
		screenSurface -> drawFrame ( r );
	}

	screenSurface -> setColor ( textColor );
	screenSurface -> setFont  ( fnt );

	int	tw, th;
	char  * str = text;
	char	buf [128];

	fnt -> size ( tw, th, text );

	for ( int py = ( h - th ) >> 1; *str != '\0'; )
	{
		for ( int i = 0; str [i] != '\n' && str [i] != '\0'; i++ )
			buf [i] = str [i];

		buf [i] = '\0';

		fnt -> size ( tw, th, buf );

		if ( style & TS_RALIGN )
			screenSurface -> drawString ( w - tw - 3, py, buf );
		else
		if ( style & TS_CENTER )
			screenSurface -> drawString ( 3 + (w - tw - 2 ) / 2, py, buf );
		else
			screenSurface -> drawString ( 3, py, buf );

		py += th;
		while ( *str != '\0' && *str != '\n' )
			str++;

		if ( *str == '\n' )
			str++;
	}

	showMouseCursor ();
}

Object * loadStaticText ()
{
	return new StaticText;
}

Object * loadStaticPic ()
{
	return new StaticPic;
}
