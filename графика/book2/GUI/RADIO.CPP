//
// RadioGroup class for GUI
// Author: Alex V. Boreskoff
//
#include        <mem.h>
#include        <stdarg.h>
#include        <stdio.h>
#include        <string.h>
#include        <dos.h>

#include        "win.h"
#include        "radio.h"
#include	"store.h"
#include        "Keys.h"

/////////////////////////////  RadioGroup  ///////////////////////////////

RadioGroup :: RadioGroup ( int x, int y, int nRows, int nCols, int cw, int ch, int ident, int v, char * s , View * p ) :
        View ( x, y, nCols * cw, nRows * ch, p )
{
        id         = ident;
        value      = v;
        rows       = nRows;
        cols       = nCols;
        cellWidth  = cw;
        cellHeight = ch;
        subFocus   = 0;
        style     |= WS_REPAINTONFOCUS;
        status    |= WS_VISIBLE | WS_ENABLED;

        delete text;

        text = new char [strlen ( s )];
        strcpy ( text, s );
}

int	RadioGroup :: put ( Store * s ) const
{
	if ( !View :: put ( s ) )
		return FALSE;

	s -> putInt ( id );
        s -> putInt ( value );
	s -> putInt ( rows );
	s -> putInt ( cols );
	s -> putInt ( cellWidth );
	s -> putInt ( cellHeight );

	return TRUE;
}

int	RadioGroup :: get ( Store * s )
{
	if ( !View :: get ( s ) )
		return FALSE;

	id         = s -> getInt ();
	value      = s -> getInt ();
	rows       = s -> getInt ();
	cols       = s -> getInt ();
	cellWidth  = s -> getInt ();
	cellHeight = s -> getInt ();

	return TRUE;
}

void    RadioGroup :: draw ( const Rect& r ) const
{
	char  * ptr = text;
        char    buf [80];

        screenSurface -> setColor ( LIGHTGRAY );
        screenSurface -> drawBar  ( 0, 0, width (), height () );

        for ( int i = 0; i < rows; i++ )
                for ( int j = 0; j < cols; j++ )
                {
                        for ( int k = 0; *ptr != '\n' && *ptr != '\0'; )
                                buf [k++] = *ptr++;

                        buf [k] = '\0';
                        if ( *ptr == '\n' )
                                ptr++;

                        drawBox ( j*cellWidth, i*cellHeight, cellWidth, cellHeight, buf, style,
                                  isFocused () && j + i*cols == subFocus, sysRadioButton [j + i*cols == subFocus ? 1 : 0] );
                }
}

int     RadioGroup :: hitTest ( const Point& p ) const
{
        return cols * (p.y / cellHeight) + (p.x / cellWidth);
}

int     RadioGroup :: keyDown ( const Message& m )
{
        if ( !isEnabled () )
                return View :: keyDown ( m );

        int     col = subFocus % cols;
        int     row = subFocus / cols;

        if ( ( m.param == UP && row < 1 ) || ( m.param == DOWN  && row >= rows - 1 ) ||
             ( m.param == LEFT  && col < 1 ) || ( m.param == RIGHT && col >= cols - 1 ) )
             return View :: keyDown ( m );

        switch ( m.param )
        {
                case ' ':
                case CR:
                        if ( subFocus != value )
                        {
                                value = subFocus;
                                repaint ();
				sendMessage ( delegate, WM_COMMAND, 0, id, this );
			}
			return TRUE;            // message processed

		case LEFT:
			if ( col > 0 )
				col --;
			break;

		case RIGHT:
			if ( col < cols - 1 )
				col++;
			break;

		case UP:
			if ( row > 0 )
				row--;
			break;

		case DOWN:
			if ( row < rows - 1 )
				row++;
			break;
	}

	subFocus = col + row * cols;
	repaint ();

	return TRUE;
}

int     RadioGroup :: mouseDown ( const Message& m )
{
	subFocus = value = hitTest ( m.mouse.loc );

	repaint ();
	sendMessage ( parent, WM_COMMAND, 0, id, this );

	return TRUE;
}

int     RadioGroup :: mouseMove ( const Message& m )
{
	if ( ( m.mouse.buttons & MK_LEFT ) != 0 && ( status & RGS_CHECKED ) == 0 )
	{
		status &= ~RGS_CHECKED;

		repaint ();
		sendMessage ( parent, WM_COMMAND, 0, id, this );
	}

	return TRUE;
}

void    drawBox ( int x, int y, int width, int height, char * text, int style, int focused, Surface * pic )
{
	int     px, py, tx, ty, w, h;
	int     picWidth  = pic -> width;
	int     picHeight = pic -> height;

	sysFont -> size ( w, h, text );

	py = y + (height - picHeight) / 2;
        px = x + (style & TS_RALIGN ? width - 1 - picWidth : 0);
        tx = x + (style & TS_RALIGN ? 0 : 1 + picWidth) + 2;
        ty = y + (height - sysFont -> height) / 2;

        if ( focused )
        {
                int     x1 = x + (style & TS_RALIGN ? 0 : picWidth + 2);
                int     x2 = x + (style & TS_RALIGN ? width - 4 - picWidth : width - 2);
                int     y1 = y + 1;
                int     y2 = y + height - 2;

                tx += 2;
                ty++;

                screenSurface -> setColor ( DARKGRAY );
                screenSurface -> drawLine ( x1, y1, x2, y1 );
                screenSurface -> drawLine ( x1, y1, x1, y2 );
                screenSurface -> setColor ( WHITE );
                screenSurface -> drawLine ( x2, y2, x1, y2 );
                screenSurface -> drawLine ( x2, y2, x2, y1 );
        }

        screenSurface -> setColor ( BLACK );
        screenSurface -> drawString ( tx, ty, text );
        pic           -> copy ( *screenSurface, Point ( px, py ) );
}

Object * loadRadioGroup ()
{
	return new RadioGroup;
}
