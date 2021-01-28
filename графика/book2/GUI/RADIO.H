//
// RadioGroup class for GUI
// Author: Alex V. Boreskoff
//
#ifndef	__RADIO__
#define	__RADIO__

#include "win.h"
#include "font.h"
			/// status bits
#define	RGS_CHECKED		0x0008

			// text styles
#define	TS_RALIGN		0x0008

///////////////////////////////////////////////////////////////////////////

class	RadioGroup : public View
{
public:
	int	id;
	int	value;		//
	int	rows;		// # of rows
	int	cols;		// # of	cols
	int	subFocus;
	int	cellWidth;
	int	cellHeight;

	RadioGroup ( int x, int y, int nRows, int nCols, int cw, int ch, int ident, int v, char * s , View * p );
	RadioGroup () : View () {}

	virtual	char  * getClassName () const
	{
		return "RadioGroup";
	}

	virtual	int	put ( Store * ) const;
	virtual	int	get ( Store * );

	virtual	int	keyDown   ( const Message& );
	virtual	int	mouseDown ( const Message& );
	virtual	int	mouseMove ( const Message& );
	virtual	void	draw      ( const Rect& ) const;
	virtual	int	hitTest   ( const Point& ) const;
};

void	drawBox ( int, int, int, int, char *, int, int, Surface * );

Object * radioGroupLoader ();

#endif