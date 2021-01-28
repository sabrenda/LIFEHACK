#include	<mem.h>
#include	<stdarg.h>
#include	<stdio.h>
#include	<string.h>
#include	<dos.h>

#include	"Win.h"
#include	"CustCtl.h"
#include	"Keys.h"

Caret :: Caret ( int size, HWND owner )
{
	wnd       = owner;
	on        = FALSE;
	lockCount = 1;
	height    = size;
}

void	Caret :: Flash ()
{
	if ( lockCount < 1 )
		Draw ();

	on = !on;
}

void	Caret :: Show ()
{
	if ( --lockCount < 1 )
	{
		lockCount = 0;
		on        = TRUE;
		Draw ();
	}
}

void	Caret :: Hide ()
{
	if ( lockCount < 1 && on )
		Draw ();

	lockCount ++;
}

void	Caret :: Move ( int px, int py )
{
	Hide ();

	x = px;
	y = py;

	Show ();
}

void	Caret :: Draw ()
{
	int	rOP = SetWriteMode ( XOR_PUT );
	int	clr = SetColor ( WHITE );

	org -> x = 0;
	org -> y = 0;
	wnd -> Local2Global  ( * org );
	wnd -> GetScreenRect ( * clipRect );

	HideMouseCursor ();
	Line            ( x, y, x, y + height - 1 );
	SetWriteMode    ( rOP );
	SetColor        ( clr & 0x00FF, clr >> 8 );
	ShowMouseCursor ();
}

//////////////////////////////// HorzLine /////////////////////////////////

void	HorzLine :: Paint ()
{
	HideMouseCursor ();

	SetColor ( style & FS_RAISED ? WHITE : DARKGRAY );
	Line     ( 0, 0, area.x2 - area.x1, 0 );
	SetColor ( style & FS_RAISED ? DARKGRAY : WHITE );
	Line     ( 0, 1, area.x2 - area.x1, 1 );

	ShowMouseCursor ();
}

//////////////////////////////// VertLine /////////////////////////////////

void	VertLine :: Paint ()
{
	HideMouseCursor ();

	SetColor ( style & FS_RAISED ? WHITE : DARKGRAY );
	Line     ( 0, 0, 0, area.y2 - area.y1 );
	SetColor ( style & FS_RAISED ? DARKGRAY : WHITE );
	Line     ( 1, 0, 1, area.y2 - area.y1 );

	ShowMouseCursor ();
}

//////////////////////////////  Frame  ////////////////////////////////////

void	Frame :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;
	Rect	r ( 0, 5, width - 1, height - 5 );
	int	tw, th;

	HideMouseCursor ();
	SetColor ( BLACK, LIGHTGRAY );
	Bar ( 0, 0, width, height );

	if ( style & FS_RAISED )
	{
		SetColor ( DARKGRAY );
		r.Move ( 1, 1 );
		DrawRect ( r );
		SetColor ( WHITE );
		r.Move ( -1, -1 );
		DrawRect ( r );
	}
	else
	{
		SetColor ( DARKGRAY );
		DrawRect ( r );
		r.Move ( 1, 1 );
		SetColor ( WHITE );
		DrawRect ( r );
	}

	SysFont -> Size ( tw, th, text );
	SetColor ( BLACK, LIGHTGRAY );

	if ( style & TS_RALIGN )
		SysFont -> PutStr ( width - 9 - tw, 0, text );
	else
	if ( style & TS_CENTER )
		SysFont -> PutStr ( ( width - 9 - tw ) >> 1, 0, text );
	else
		SysFont -> PutStr ( 10, 0, text );

	ShowMouseCursor ();
}

////////////////////////////// StaticText /////////////////////////////////

StaticText :: StaticText ( int x, int y, int w, int h, HWND p, char * t, char tc, char bc, char br, Font * fn, int del ) :
		GenericWindow ( x, y, w, h, p )
{
	strcpy ( text, t );
	backColor    = bc;
	textColor    = tc;
	borderType   = br;
	fnt          = fn;
	shouldDelete = del;
	status      |= WS_VISIBLE;
}

void	StaticText :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;

	HideMouseCursor ();

	if ( borderType == SS_GRAY )
	{
		SetColor ( LIGHTGRAY, LIGHTGRAY );
		Line ( 0, 0, width, 0 );
		Line ( 0, 0, 0, height );
		Line ( 0, height, width, height );
		Line ( width, 0, width, height );
		SetColor ( textColor, backColor );
		Bar ( 1, 1, width - 1, height - 1 );
	}
	else
	if ( borderType == SS_BLACK )
	{
		SetColor ( BLACK, LIGHTGRAY );
		Line ( 0, 0, width, 0 );
		Line ( 0, 0, 0, height );
		Line ( 0, height, width, height );
		Line ( width, 0, width, height );
		SetColor ( textColor, backColor );
		Bar ( 1, 1, width - 1, height - 1 );
	}
	else
	if ( borderType == SS_3D )
	{
		SetColor ( WHITE );
		Line ( 0,         height, width, height );
		Line ( width, 0,          width, height );

		SetColor ( LIGHTGRAY );
		Line ( 1,         height - 1, width - 1, height - 1 );
		Line ( width - 1, 1,          width - 1, height - 1 );

		SetColor ( DARKGRAY );
		Line ( 0, 0, width - 1, 0 );
		Line ( 0, 0, 0,         height - 1 );

		if ( backColor == DARKGRAY )
			SetColor ( BLACK );

		Line ( 1, 1, width - 2, 1 );
		Line ( 1, 1, 1,         height - 2 );

		SetColor ( textColor, backColor );
		Bar ( 2, 2, width - 2, height - 2 );
	}
	else
	if ( borderType == SS_DOWN )
	{
		SetColor ( DARKGRAY, backColor );
		Bar ( 1, 1, width - 1, height - 1 );
		Line ( 0, 0, width, 0 );
		Line ( 0, 0, 0, height );
		SetColor ( WHITE, backColor );
		Line ( width, height, 1, height );
		Line ( width, height, width, 1 );
		SetColor ( textColor, backColor );
	}
	else
	if ( borderType == SS_UP )
	{
		SetColor ( DARKGRAY, backColor );
		Bar ( 1, 1, width - 1, height - 1 );
		SetColor ( WHITE, backColor );
		Line ( 0, 0, width, 0 );
		Line ( 0, 0, 0, height );
		SetColor ( textColor, backColor );
		Line ( width, height, 1, height );
		Line ( width, height, width, 1 );
		SetColor ( textColor, backColor );
	}
	else
	if ( borderType == SS_FRAMED )
	{
		Rect	r ( 0, 0, width - 1, height - 1 );

		SetColor ( DARKGRAY, backColor );
		Bar ( 0, 0, width, height );
		DrawRect ( r );
		r.Move ( 1, 1 );
		SetColor ( WHITE );
		DrawRect ( r );
		SetColor ( textColor, backColor );
	}
	else
	if ( borderType == SS_FRAMERAISED )
	{
		Rect	r ( 1, 1, width, height );

		SetColor ( DARKGRAY, backColor );
		Bar ( 0, 0, width, height );
		DrawRect ( r );
		r.Move ( -1, -1 );
		SetColor ( WHITE );
		DrawRect ( r );
		SetColor ( textColor, backColor );
	}

	int	w, h;
	char  * str = text;

	fnt -> ExtSize ( w, h, text );

	for ( int py = ( height - h ) >> 1; *str != '\0'; )
	{
		fnt -> Size ( w, h, str );
		if ( style & TS_RALIGN )
			fnt -> PutStr ( width - w - 3, py, str );
		else
		if ( style & TS_CENTER )
			fnt -> PutStr ( 3 + ( width - w - 2 ) / 2, py, str );
		else
			fnt -> PutStr ( 3, py, str );

		py += h;
		while ( *str != '\0' && *str != '\n' )
			str++;

		if ( *str == '\n' )
			str++;
	}

	ShowMouseCursor ();
}

/////////////////////////////  Indicator //////////////////////////////////

Indicator :: Indicator ( int x, int y, int w, long mValue, HWND p ) : GenericWindow ( x, y, w, SM_INDICATOR, p )
{
	numLights    = w / SM_INDICATOR;
	activeLights = 0;
	status      |= WS_VISIBLE;
	maxValue     = mValue;
	curValue     = 0;
}

void	Indicator :: SetValue ( long val )
{
	if ( val > maxValue )
		val = maxValue;
	else
	if ( val < 0 )
		val = 0;

	if ( val == curValue )
		return;

	curValue     = val;
	activeLights = ( numLights * val ) / maxValue;
	Repaint ();
}

void	Indicator :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;
	int	startX = ( width - numLights*SM_INDICATOR ) >> 1;

	HideMouseCursor ();
	SetColor ( DARKGRAY, LIGHTGRAY );
	Line ( 0, 0, width, 0 );
	Line ( 0, 0, 0, height );
	SetColor ( WHITE, LIGHTGRAY );
	Line ( width, height, width, 0 );
	Line ( width, height, 0, height );

	if ( startX > 1 )
		Bar ( 1, 1, startX - 1, height - 1 );

	if ( startX + numLights*SM_INDICATOR < width )
		Bar ( startX + numLights*SM_INDICATOR, 1, width - 1, height - 1 );

	for ( int i = 0; i < numLights; i++ )
		Ind [ i < activeLights ? 1 : 0 ] -> Put ( startX + i*SM_INDICATOR, 0 );

	ShowMouseCursor ();
}

//////////////////////////////  EditLine //////////////////////////////////

EditLine :: EditLine ( int x, int y, int w, int h, HWND p, char * t, char bc, char tc, char br, Font * fn, int del ) :
		StaticText ( x, y, w, h, p, t, bc, tc, br, fn, del )
{
	insState = FALSE;
	pos      = 0;
	len      = strlen ( text );
	caret    = new Caret ( fnt -> Height, (HWND) this );
	status  |= WS_VISIBLE | WS_ENABLED;

	caret -> x = GetPos ( pos );
	caret -> y = ( area.y2 - area.y1 + 1 - fnt -> Height ) / 2;
}

inline void EditLine :: SetPos ( int p )
{
	caret -> Move ( GetPos ( pos = p ), caret -> y );
}

int	EditLine :: QueryUseKey ( int key )
{
	return ( key == LEFT && pos > 0 ) || ( key == RIGHT && pos < len ) || key == HOME || key == END ||
		 key == DEL  || key == BS    || key == INS  || ( key >= ' ' && key <= 0xFF );
}

void	EditLine :: Paint ()
{
	caret -> Hide ();
	StaticText :: Paint ();
	caret -> Show ();
}

long	EditLine :: Handle ( Message& m )
{
	if ( m.msg == WM_FOCUS )
	{
		if ( m.wParam )
			caret -> Show ();
		else
			caret -> Hide ();
	}
	else
	if ( m.msg == WM_CURSORCLICK )
	{
		caret -> Flash ();
		return 0;
	}
	else
	if ( m.msg == WM_CHAR )
	{
		switch ( m.wParam )
		{
			case INS:
				insState = !insState;
				break;

			case LEFT:
				if ( pos > 0 )
				{
					pos --;
					caret -> Move ( caret -> x - fnt -> CharSize ( text [pos] ), caret -> y );
				}
				break;

			case RIGHT:
				if ( pos < len )
				{
					caret -> Move ( caret -> x + fnt -> CharSize ( text [pos] ), caret -> y );
					pos++;
				}
				break;

			case BS:
				if ( pos == 0 )
					break;
				pos --;
						// fall through
			case DEL:
				if ( pos < len )
				{
					for ( int i = pos; i < len; i++ )
						text [i] = text [i+1];

					len--;
					SetPos ( pos );
					Repaint ();
				}
				break;

			case HOME:
				SetPos ( 0 );
				break;

			case END:
				SetPos ( len );
				break;

			default:
				if ( m.wParam >= 32 && m.wParam <= 255 && Filter ( (char) m.wParam ) )
				{
					for ( int i = len + 1; i > pos; i-- )
						text [i] = text [i-1];

					text [i]   = (char) m.wParam;
					caret -> Move ( caret -> x + fnt -> CharSize ( text [pos] ), caret -> y );
					pos++;
					len++;
					Repaint ();
					SendMessage ( parent, WM_COMMAND, LEN_TEXTCHANGED, (long) this );
				}
				else
					MessageBeep ();
		}
	}

	return GenericWindow :: Handle ( m );
}

int	EditLine :: GetPos ( int p )
{
	for ( int i = 0, v = 3; i < p; i++ )
		v += fnt -> CharSize ( text [i] );

	return v;
}

////////////////////////////////// PushButton /////////////////////////////////

int	PushButton :: QueryUseKey ( int key )
{
	return key == ' ' || key == CR;
}

long	PushButton :: Handle ( Message& m )
{
	if ( !IsEnabled () )
		return 0;

	switch ( m.msg )
	{
		case WM_CHAR:
			if ( m.wParam == ' ' || m.wParam == CR )
			{
				status |= BS_PRESSED;
				Repaint ();
				delay   ( 100 );
				status &= ~BS_PRESSED;
				Repaint ();
				SendMessage ( parent, WM_COMMAND, id, (long) this );
				return 0;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_DBLCLICK:
			if ( Contains ( m.mouse.loc ) )
			{
				status |= BS_PRESSED | BS_TRACKING;
				Repaint ();
				CaptureMouse ( this );
			}
			break;

		case WM_LBUTTONUP:
			ReleaseMouse ();
			if ( status & BS_TRACKING )
			{
				status &= ~BS_TRACKING;

				if ( Contains ( m.mouse.loc ) )
				{
					status &= ~BS_PRESSED;
					Repaint ();
					SendMessage ( parent, WM_COMMAND, id, (long) this );
					return 0;
				}
			}

			return 0;		// dont want to do anything since after SendMessage
						// object can be destroyed

		case WM_MOUSEMOVE:
			if ( ( m.mouse.buttons & MK_LEFT ) && status & BS_TRACKING )
				if ( Contains ( m.mouse.loc ) )
				{
					if ( ! ( status & BS_PRESSED ) )
					{
						status |= BS_PRESSED;
						Repaint ();
					}
				}
				else
					if ( status & BS_PRESSED )
					{
						status &= ~BS_PRESSED;
						Repaint ();
					}
			break;
	}

	return GenericWindow :: Handle ( m );
}

////////////////////////////////// TextButton //////////////////////////////

void	TextButton :: Paint ()
{
	int	w = Width () - 1;
	int	h = Height () - 1;
	int	tw, th;

	if ( status & BS_PRESSED )
	{
		SetColor ( BLACK, LIGHTGRAY );
		Bar  ( 0, 0, w, h );
		Line ( 0, 0, w - 1, 0 );
		Line ( w - 1, 0, w - 1, h - 1 );
		Line ( w - 1, h - 1, 0, h - 1 );
		Line ( 0, h - 1, 0, 0 );
		SetColor ( DARKGRAY, LIGHTGRAY );
		Line ( 1, 1, w - 2, 1 );
		Line ( 1, 1, 1, h - 2 );
		SetColor ( BLACK, LIGHTGRAY );

		SysFont -> Size ( tw, th, text );
		SysFont -> PutStr ( 2 + ( ( w - tw ) >> 1 ), 1 + ( ( h - th ) >> 1 ), text );
	}
	else
	{
		SetColor ( WHITE, LIGHTGRAY );
		Bar      ( 0, 0, w, h );
		Line     ( 0, 0, w, 0 );
		Line     ( 0, 0, 0, h );
		SetColor ( DARKGRAY, LIGHTGRAY );
		Line     ( 1, h - 1, w - 1, h - 1 );
		Line     ( w - 1, 1, w - 1, h - 1 );
		SetColor ( BLACK, LIGHTGRAY );
		Line     ( 0, h, w, h );
		Line     ( w, 0, w, h );

		SysFont -> Size ( tw, th, text );
		SysFont -> PutStr ( ( w - tw ) >> 1, ( h - th ) >> 1, text );
	}
}

////////////////////////////////// PicButton ///////////////////////////////

PicButton :: PicButton ( int x, int y, HWND p, BitMap * Up, BitMap * Down, BitMap * Focused, int ident )
	: PushButton ( x, y, Up -> Width, Up -> Height, p, ident )
{
	face [0] = Up;
	face [1] = Down;
	face [2] = Focused;
	status  |= WS_ENABLED | WS_REPAINTONFOCUS | WS_VISIBLE;
}

PicButton :: ~PicButton ()
{
	delete face [0];
	delete face [1];
	delete face [2];
}

void	PicButton :: Paint ()
{
	if ( !IsEnabled () )
		face [0] -> PutDisabled ( 0, 0 );
	else
	if ( status & BS_PRESSED )
		face [1] -> Put ( 0, 0 );
	else
	if ( focus == (HWND) this )
		face [2] -> Put ( 0, 0 );
	else
		face [0] -> Put ( 0, 0 );
}

/////////////////////////////  CheckBox  //////////////////////////////////

void	CheckBox :: Paint ()
{
	int	n = status & BS_CHECKED ? 1 : 0;

	DrawBox ( 0, 0, Width (), Height (), text, style, focus == (HWND) this, SysCheckBox [n] );
}

int	CheckBox :: QueryUseKey ( int key )
{
	return key == ' ';
}

long	CheckBox :: Handle ( Message& m )
{
	switch ( m.msg )
	{
		case	WM_LBUTTONDOWN:
			if ( ( status & BS_PRESSED ) == 0 )
			{
				CaptureMouse ( this );
				status |= BS_PRESSED;
				Repaint ();
			}

			return 0;

		case WM_MOUSEMOVE:
			if ( m.mouse.buttons & MK_LBUTTON && !Contains ( m.mouse.loc ) )
			{
				status &= ~BS_PRESSED;
				ReleaseMouse ();
				Repaint ();
			}

			return 0;

		case WM_LBUTTONUP:
			if ( status & BS_PRESSED )
			{
				status &= ~BS_PRESSED;
				status ^=  BS_CHECKED;
				ReleaseMouse ();
				Repaint ();
				SendMessage ( parent, WM_COMMAND, id, (long) this );
			}

			return 0;

		case	WM_CHAR:
			if ( m.wParam != 32 )
				break;

			status ^= BS_CHECKED;
			Repaint ();
			SendMessage ( parent, WM_COMMAND, id, (long) this );

			return 0;
	}

	return GenericWindow :: Handle ( m );
}

/////////////////////////////  RadioButton  ///////////////////////////////

RadioGroup :: RadioGroup ( int x, int y, int nRows, int nCols, int cw, int ch, int ident, int v, char * s , HWND p ) :
	GenericWindow ( x, y, nCols * cw, nRows * ch, p )
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

void	RadioGroup :: Paint ()
{
	char  * ptr = text;
	char	buf [80];
	int	isFocused = focus == (HWND) this;

	SetColor ( BLACK, LIGHTGREEN );
	Bar ( 0, 0, Width (), Height () );

	for ( int i = 0; i < rows; i++ )
		for ( int j = 0; j < cols; j++ )
		{
			for ( int k = 0; *ptr != '\n' && *ptr != '\0'; )
				buf [k++] = *ptr++;

			buf [k] = '\0';
			if ( *ptr == '\n' )
				ptr++;

			DrawBox ( j*cellWidth, i*cellHeight, cellWidth, cellHeight, buf, style,
				  isFocused && j + i*cols == subFocus, SysRadioButton [j + i*cols == subFocus ? 1 : 0] );
		}
}

int	RadioGroup :: QueryUseKey ( int key )
{
	int	col = subFocus % cols;
	int	row = subFocus / cols;

	return key == ' ' || ( key == UP && row > 0 ) || ( key == DOWN  && row < rows - 1 ) ||
	       ( key == LEFT  && col > 0 ) || ( key == RIGHT && col < cols - 1 );
}

int	RadioGroup :: HitTest ( Point& p )
{
	return cols * (p.y / cellHeight) + (p.x / cellWidth);
}

long	RadioGroup :: Handle ( Message& m )
{
	int	col = subFocus % cols;
	int	row = subFocus / cols;

	switch ( m.msg )
	{
		case	WM_CHAR:
			switch ( m.wParam )
			{
				case ' ':
				case CR:
					if ( subFocus != value )
					{
						value = subFocus;
						Repaint ();
						SendMessage ( parent, WM_COMMAND, id, (long)this );
					}
					break;

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
			Repaint ();
			break;

		case	WM_LBUTTONDOWN:
			subFocus = value = HitTest ( m.mouse.loc );
			col      = subFocus % cols;
			row      = subFocus / cols;
			Repaint ();
			SendMessage ( parent, WM_COMMAND, id, (long)this );
			return 0;

		case WM_MOUSEMOVE:
			if ( ( m.wParam & 0x0001 ) != 0 && ( status & BS_CHECKED ) == 0 )
			{
				status &= ~BS_CHECKED;
				Repaint ();

				SendMessage ( parent, WM_COMMAND, id, (long) this );
			}

			return 0;
	}

	return GenericWindow :: Handle ( m );
}

///////////////////////////////  ScrollBar ////////////////////////////////

void	ScrollBar :: SetValue ( int v )
{
	if ( v < minValue )
		v = minValue;

	if ( v > maxValue )
		v = maxValue;

	curValue  = v;
	markerPos = ( (curValue + pageSize - minValue) * ScrollerLen () ) / (maxValue - minValue + pageSize) - markerSize;

	if ( markerPos + markerSize >= ScrollerLen () )
		markerPos = ScrollerLen () - markerSize - 1;

	if ( markerPos < 0 )
		markerPos = 0;

	Repaint ();
}

void	ScrollBar :: SetRange ( int Min, int Max, int Size )
{
	if ( Min > Max )
		Max = Min;

	maxValue = Max;
	minValue = Min;
	pageSize = Size;

	ComputeMarker ();

	SetValue ( curValue );
}

void	ScrollBar :: ComputeMarker ()
{
	if ( ( markerSize = (pageSize * ScrollerLen ()) / (maxValue - minValue + pageSize) ) < 15 )
		markerSize = 15;
}

void	ScrollBar :: RepaintButton ( int btn, int cond )
{
	org -> x = 0;
	org -> y = 0;
	Local2Global  ( * org );
	GetScreenRect ( * clipRect );
	* clipRect &= * screenRect;
	HideMouseCursor ();

	switch ( btn )
	{
		case 0:
			SysArrows [ cond ? 4 : 0 ] -> Put ( 2, Height () - 2*16 - 2 );
			break;

		case 1:
			SysArrows [ cond ? 5 : 1 ] -> Put ( 2, Height () - 18 );
			break;

		case 2:
			SysArrows [ cond ? 6 : 2 ] -> Put ( 2, Height () - 2*16 - 2 );
			break;

		case 3:
			SysArrows [ cond ? 7 : 3 ] -> Put ( Width () - 18, 2 );
	}

	if ( cond )
	{
		status |= SBS_BUTTONPRESSED;
		CaptureMouse ( this );
	}
	else
	{
		status &= ~SBS_BUTTONPRESSED;
		ReleaseMouse ();
	}

	ShowMouseCursor ();
}

long	ScrollBar :: Handle ( Message& m )
{
	int	code = HitTest ( m.mouse.loc );

	if ( minValue == maxValue )
		return 0;

	switch ( m.msg )
	{
		case WM_LBUTTONDOWN:
			if ( code == SB_TRACK )
			{
				partCode = SB_TRACK;
				Track ( m.mouse.loc );

				return 0;
			}

			if ( code == SB_LINEUP )
			{
				RepaintButton ( upButton, TRUE );
				partCode = SB_LINEUP;

				return 0;
			}

			if ( code == SB_LINEDOWN )
			{
				RepaintButton ( downButton, TRUE );
				partCode = SB_LINEDOWN;
				return 0;
			}

			partCode = code;
			return 0;

		case WM_MOUSEMOVE:
			if ( partCode != code )
			{
				if ( partCode == SB_LINEUP )
					RepaintButton ( upButton, FALSE );
				else
				if ( partCode == SB_LINEDOWN )
					RepaintButton ( downButton, FALSE );

				partCode = code;
			}
			return 0;

		case WM_LBUTTONUP:
			if ( code == SB_LINEUP )
				RepaintButton ( upButton, FALSE );
			else
			if ( code == SB_LINEDOWN )
				RepaintButton ( downButton, FALSE );

		case WM_AUTOCLICK:
			SendMessage ( parent, scrollMsg, partCode = code );
			return 0;
	}

	return GenericWindow :: Handle ( m );
}

void	ScrollBar :: Track ( Point& p )
{
	Message m;

	HideMouseCursor ();

	do
	{
		GetMessage ( m );
		if ( IsMouseMessage ( m ) )
		{
			Global2Local ( m.mouse.loc );

			markerPos += (scrollMsg == WM_VSCROLL ? m.mouse.loc.y - p.y : m.mouse.loc.x - p.x);

			if ( markerPos + markerSize >= ScrollerLen () )
				markerPos = ScrollerLen () - markerSize - 1;

			if ( markerPos < 0 )
				markerPos = 0;

			Repaint ();
			SendMessage ( parent, scrollMsg, SB_THUMBTRACK, (long) this );
			p = m.mouse.loc;
		}
	} while ( m.msg != WM_LBUTTONUP );

	ShowMouseCursor ();
	SendMessage ( parent, scrollMsg, SB_THUMBPOS, (long) this );
}

////////////////////////////  VScrollBar  ////////////////////////////////

VScrollBar :: VScrollBar ( int x, int y, int h, HWND p ) : ScrollBar ( x, y, SM_SCROLLBARSIZE, h, p )
{
	strcpy ( text, "VertScrollBar" );
	scrollMsg  = WM_VSCROLL;
	upButton   = 0;
	downButton = 1;
	status    |= WS_VISIBLE | WS_ENABLED;
}

int	VScrollBar :: ScrollerLen ()
{
	return Height () - 4 - 2*16;
}


int	VScrollBar :: HitTest ( Point& p )
{
	int	height = area.y2 - area.y1;

	if ( p.x < 1 || p.x > 16 || p.y < 2 )
		return SB_NOACTION;

	if ( p.y >= 2 && p.y < markerPos )
		return SB_PAGEUP;

	if ( p.y >= 2 + markerPos && p.y < 2 + markerPos + markerSize )
		return SB_TRACK;

	if ( p.y >= 2 + markerPos + markerSize && p.y < height - 2*16 )
		return SB_PAGEDOWN;

	if ( p.y >= height - 2*16 && p.y < height - 16 )
		return SB_LINEUP;
	else
		return SB_LINEDOWN;
}

void	VScrollBar :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;
	Rect	r ( 0, 0, width, height );

	if ( minValue == maxValue )
	{
		SetColor ( BLACK, DARKGRAY );
		DrawRect ( r );
		r.Grow ( -1, -1 );
		SetColor ( LIGHTGRAY, DARKGRAY );
		DrawRect ( r );
		SetFillPattern ( DOT_FILL );
		FillBar ( 2, 2, width - 2, height - 2 );
		SetFillPattern ( SOLID_FILL );
	}
	else
	{
		if ( partCode == SB_LINEUP && status & SBS_BUTTONPRESSED )
			SysArrows [4] -> Put ( 2, height - 2*16 - 1 );
		else
			SysArrows [0] -> Put ( 2, height - 2*16 - 1 );

		if ( partCode == SB_LINEDOWN && status & SBS_BUTTONPRESSED )
			SysArrows [5] -> Put ( 2, height - 16 - 1 );
		else
			SysArrows [1] -> Put ( 2, height - 16 - 1 );

		SetColor ( BLACK, DARKGRAY );
		DrawRect ( r );
		SetColor ( LIGHTGRAY, DARKGRAY );
		r.Grow ( - 1, -1 );
		DrawRect ( r );
		SetFillPattern ( DOT_FILL );

		if ( markerPos > 0 )
			FillBar ( 2, 2, width - 2, 1 + markerPos );

		if ( markerPos + markerSize < height - 2*16 )
			FillBar ( 2, 2 + markerPos + markerSize, width - 2, height - 2*16 - 2 );

		SetFillPattern ( NULL );

		SetColor ( WHITE );	// now draw the marker
		Line ( 2, 2 + markerPos, 16, 2 + markerPos );
		Line ( 2, 2 + markerPos, 2,  markerPos + markerSize );

		SetColor ( BLACK );
		Line ( 17, 2 + markerPos,              17, 2 + markerPos + markerSize );
		Line ( 3,  2 + markerPos + markerSize, 16, 2 + markerPos + markerSize );

		SetColor ( DARKGRAY, LIGHTGRAY );
		Line ( 16, 2 + markerPos,              16, markerPos + markerSize );
		Line ( 4,  2 + markerPos + markerSize - 1, 17, 2 + markerPos + markerSize - 1 );
		Bar  ( 3,  3 + markerPos, 	       15, 2 + markerPos + markerSize - 2 );

		Marker -> Put ( 5, 2 + markerPos + ( markerSize - 10 ) / 2 );
	}
}


////////////////////////////  HScrollBar  ////////////////////////////////

HScrollBar :: HScrollBar ( int x0, int y0, int w, HWND p ) : ScrollBar ( x0, y0, w, SM_SCROLLBARSIZE, p )
{
	strcpy ( text, "HorzScrollBar" );
	scrollMsg  = WM_HSCROLL;
	upButton   = 2;
	downButton = 3;
	status    |= WS_VISIBLE | WS_ENABLED;
}

int	HScrollBar :: ScrollerLen ()
{
	return Width () - 4 - 2*16;
}

int	HScrollBar :: HitTest ( Point& p )
{
	int	width = area.x2 - area.x1;

	if ( p.y < 1 || p.y > 16 || p.x < 2 )
		return SB_NOACTION;

	if ( p.x >= 2 && p.x < 2 + 16 )
		return SB_LINEUP;

	if ( p.x >= 2 + 16 && p.x < 2 + 2*16 )
		return SB_LINEDOWN;

	if ( p.x > 2 + 2*16 + markerPos && p.x < 2 + 2*16 + markerPos + markerSize )
		return SB_TRACK;

	if ( p.x < 2 + 2*16 + markerPos )
		return SB_PAGEUP;

	if ( p.x > 2 + 2*16 + markerPos + markerSize )
		return SB_PAGEDOWN;
	else
		return SB_NOACTION;
}

void	HScrollBar :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;
	Rect	r ( 0, 0, width, height );

	if ( minValue == maxValue )
	{
		SetColor ( BLACK, DARKGRAY );
		DrawRect ( r );
		SetColor ( LIGHTGRAY, DARKGRAY );
		r.Grow ( - 1, - 1 );
		DrawRect ( r );
		SetFillPattern ( DOT_FILL );
		FillBar ( 2, 2, width - 2, height - 2 );
		SetFillPattern ( SOLID_FILL );
	}
	else
	{
		SysArrows [2] -> Put ( 2, 2 );
		SysArrows [3] -> Put ( 2 + 16, 2 );

		SetColor ( BLACK );
		DrawRect ( r );
		SetColor ( LIGHTGRAY, DARKGRAY );
		r.Grow ( -1, - 1 );
		DrawRect ( r );
		SetFillPattern ( (char far *) DOT_FILL );

		if ( markerPos > 0 )
			FillBar ( 2 + 2*16, 2, 2 + 2*16 + markerPos, height - 2 );

		if ( 2 + 2*16 + markerPos + markerSize < width - 2 )
			FillBar ( 2 + 2*16 + markerPos + markerSize, 2, width - 2, height - 2 );

		SetFillPattern ( SOLID_FILL );

		SetColor ( WHITE );	// now draw the marker
		Line ( 2 + 2*16 + markerPos, 2, 2 + 2*16 + markerPos, 17 );
		Line ( 2 + 2*16 + markerPos, 2, 2 + 2*16 + markerPos + markerSize - 1, 2 );

		SetColor ( BLACK );
		Line ( 2 + 2*16 + markerPos,              17, 2 + 2*16 + markerPos + markerSize, 17 );
		Line ( 2 + 2*16 + markerPos + markerSize, 3,  2 + 2*16 + markerPos + markerSize, 16 );

		SetColor ( DARKGRAY, LIGHTGRAY );
		Line ( 2 + 2*16 + markerPos + 1,              16, 2+2*16 + markerPos + markerSize - 2, 16 );
		Line ( 2 + 2*16 + markerPos + markerSize - 1, 4,  2 + 2*16 + markerPos + markerSize - 1, 17 );
		Bar  ( 2 + 2*16 + markerPos + 1, 		3,  2 + 2*16 + markerPos + markerSize - 2, 15 );

		Marker -> Put ( 2 + 2*16 + markerPos + ( markerSize - 10 ) / 2, 5 );
	}
}

/////////////////////////////  ListBox  ////////////////////////////////////
/*
ListBox :: ListBox ( int x, int y, int w, int h, HWND p, int ih ) : GenericWindow ( x, y, w, h, p )
{
	Scroller   = new VScrollBar ( 0, 0, area.y2 - area.y1 + 1, (HWND) this );
	Top        = 0;
	Current    = 0;
	ItemHeight = ih;
	PageSize   = ( area.y2 - area.y1 - 1 ) / ItemHeight;
	Style     |= WS_ENABLED | WS_REPAINTONFOCUS;
}

void	ListBox :: Paint ()
{
	int	width  = area.x2 - area.x1;
	int	height = area.y2 - area.y1;
	Rect	r ( 1, 1, width - 1, height - 1 );
	int	ItemState = ( Focus == (HWND) this ? IS_FOCUSED : 0 ) | IS_SELECTED;

	HideMouse ();

	SetColor ( BLACK );
	Line ( 0, 0, width, 0 );

	SetColor ( WHITE, LIGHTGRAY );
	Line ( width, 0,      width, height - 1 );
	Line ( 1,     height, width, height );

	Bar ( 18, 1, width - 1, height - 1 );

	r.Move ( Org -> x, Org -> y );
	* Clip &= r;

	for ( int i = Top; i < GetCount () && ( i - Top ) * ItemHeight <= height; i++ )
		DrawItem ( 18, 1 + ( i - Top ) * ItemHeight, i, i == Current ? ItemState : 0 );

	ShowMouse ();
}

int	ListBox :: Handle ( Message& m )
{
	if ( m.Msg == NM_FOCUS && m.wParam )
	{
		SetFocus ( (HWND)this );

		return 0;
	}
	else
	if ( m.Msg == WM_VSCROLL )
	{
		if ( m.wParam == SB_LINEUP && Top > 0 )
		{
			Top--;
			Repaint ();
			Scroller -> Up ();
		}
		else
		if ( m.wParam == SB_LINEDOWN && Top + PageSize < GetCount () )
		{
			Top++;
			Repaint ();
			Scroller -> Down ();
		}
		else
		if ( m.wParam == SB_PAGEUP && Top > 0 )
		{
			if ( ( Top -= PageSize ) < 0 )
				Top = 0;

			Scroller -> SetValue ( Top );
			Repaint ();
		}
		else
		if ( m.wParam == SB_PAGEDOWN && Top + PageSize < GetCount () )
		{
			if ( ( Top += PageSize ) + PageSize > GetCount () )
				Top = GetCount () - PageSize;
			if ( Top < 0 )
				Top = 0;

			Scroller -> SetValue ( Top );
			Repaint ();
		}
	}
	else
	if ( m.Msg == WM_DBLCLICK )
	{
		SetActive ( FindItem ( m.MouseLoc ) );
		SendMessage ( Parent, WM_COMMAND, LBN_DBLCLICK, (long) this );
	}
	if ( m.Msg == WM_LBUTTONDOWN )
		SetActive ( FindItem ( m.MouseLoc ) );
	else
	if ( m.Msg == WM_MOUSEMOVE && m.wParam & MK_LBUTTON )
		SetActive ( FindItem ( m.MouseLoc ) );
	else
	if ( m.Msg == WM_CHAR )
	{
		switch ( m.wParam )
		{
			case ' ':
				SendMessage ( Parent, WM_COMMAND, LBN_DBLCLICK, (long) this );
				break;

			case UP:
				if ( Current > 0 )
					if ( Current - 1 < Top )
					{
						Current--;
						Top--;
						Repaint ();
						Scroller -> Up ();
					}
					else
						SetActive ( Current - 1 );
				break;

			case DOWN:
				if ( Current < GetCount () - 1 )
					if ( ( Current - Top + 2 ) * ItemHeight >= area.y2 - area.y1 )
					{
						Current++;
						Top ++;
						Repaint ();
						Scroller -> Down ();
					}
					else
						SetActive ( Current + 1 );

				break;

			case HOME:
				if ( Current > 0 )
				{
					if ( Top > 0 )
					{
						Current = Top = 0;
						Scroller -> SetValue ( 0 );
					}
					else
						SetActive ( 0 );

					Repaint ();
				}
				break;

			case END:
				Current = GetCount () - 1;
				Top     = GetCount () - PageSize;
				if ( Top < 0 )
					Top = 0;

				Scroller -> SetValue ( Top );
				Repaint ();
				break;
		}
	}

	return 0;
}

void	ListBox :: SetActive ( int index )
{
	if ( index >= GetCount () )
		index = GetCount () - 1;

	if ( index < 0 )
		index = 0;

	if ( index == Current )
		return;

	Current = index;
	Repaint ();

	SendMessage ( Parent, WM_COMMAND, LBN_SELCHANGE, (long) this );
}

int	ListBox :: FindItem ( Point& p )
{
	int	i = Top;
	int	top = 1;

	if ( p.y < 1 )
		return Current;

	while ( p.y >= top )
	{
		i++;
		top += ItemHeight;

		if ( i > GetCount () )
			return Current;
	}

	return i - 1;
}

void	ListBox :: ResetScroller ()
{
	Scroller -> CurValue = Top;
	Scroller -> SetRange ( 0, GetCount () - PageSize, PageSize );
}

///////////////////////////////////////////////////////////////////////////

void	OkBox ( char * Caption, char * Msg ... )
{
	char	* buf = new char [1024];
	int	  Width, Height;
	va_list	 arg;

	va_start ( arg, Msg );
	vsprintf ( buf, Msg, arg );
	va_end ( arg );

	SysFont -> ExtSize ( Width, Height, buf );

	if ( Width < 150 )
		Width = 150;

	DialogWindow	* Dlg = new DialogWindow ( 200, 100, 100 + Width, 100 + Height, Caption, Bottom );

	new StaticPic ( 10, 30, Dlg, new BitMap ( "EXCLAM.BM" ) );
	new Button    ( ( 100+Width-60)/2, 50 + Height, Dlg, new BitMap ( "OK0.BM" ), new BitMap ( "OK1.BM" ), new BitMap ( "OK2.BM" ), 1 );

	StaticText * Txt = new StaticText ( 70, 30, Width + 10, Height + 10, Dlg, "", DARKGRAY, WHITE, SS_3D );

	Txt -> SetText ( buf );
	Txt -> Style |= TS_CENTER;
	Dlg -> Style &= ~WS_RESIZEABLE;
	Dlg -> SetDefID ( IDOK );
	Dlg -> Execute ();

	delete buf;
	delete Dlg;
}
*/
void	DrawBox ( int x, int y, int width, int height, char * Text, unsigned Style, int Focused, BitMap * Pic )
{
	int	px, py, tx, ty, w, h;
	int	PicWidth  = Pic -> Width;
	int	PicHeight = Pic -> Height;

	HideMouseCursor ();
	SetColor ( BLACK, LIGHTGRAY );
	Bar ( x, y, x + width - 1, y + height - 1 );

	SysFont -> Size ( w, h, Text );

	py = y + (height - PicHeight) / 2;
	px = x + (Style & TS_RALIGN ? width - 1 - PicWidth : 0);
	tx = x + (Style & TS_RALIGN ? 0 : 1 + PicWidth) + 2;
	ty = y + (height - SysFont -> Height) / 2;

	if ( Focused )
	{
		int	x1 = x + (Style & TS_RALIGN ? 0 : PicWidth + 2);
		int	x2 = x + (Style & TS_RALIGN ? width - 4 - PicWidth : width - 2);
		int	y1 = y + 1;
		int	y2 = y + height - 2;

		tx += 2;
		ty++;

		SetColor ( DARKGRAY, LIGHTGRAY );
		Line ( x1, y1, x2, y1 );
		Line ( x1, y1, x1, y2 );
		SetColor ( WHITE, LIGHTGRAY );
		Line ( x2, y2, x1, y2 );
		Line ( x2, y2, x2, y1 );
		SetColor ( BLACK, LIGHTGRAY );
	}

	SysFont -> PutStr ( tx, ty, Text );
	Pic     -> Put ( px, py );

	ShowMouseCursor ();
}
