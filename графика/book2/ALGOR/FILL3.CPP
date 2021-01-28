#include	<conio.h>
#include	<graphics.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>

#define	BLOCKED		1
#define	UNBLOCKED	2
#define	FALSE		0
#define	TRUE		0xFFFF

int	borderColor = WHITE;
int	fillColor   = GREEN;

struct	BPStruct			// table of border pixels
{
	int	x, y;
	int	flag;
} bp [3000];

int		bpStart;
int		bpEnd = 0;
BPStruct	currentPixel;
int		D;			// current search direction
int		prevD;			// prev. search direction
int		prevV;			// prev. vertical direction

/////////////////////////////////////////////////////////////////////////////


void	appendBPList ( int x, int y, int flag )
{
	bp [bpEnd  ].x    = x;
	bp [bpEnd  ].y    = y;
	bp [bpEnd++].flag = flag;
}

void	sameDirection ()
{
	if ( prevD == 0 )			// moving right
		bp [bpEnd-1].flag = BLOCKED;	// block previous pixel
	else
	if ( prevD != 4 )			// if not moving horizontally
		appendBPList ( currentPixel.x, currentPixel.y, UNBLOCKED );
}

void	differentDirection ( int d )
{
	if ( prevD == 4 )			// previously moving left
	{
		if ( prevV == 5 )		// if from above block rightmost in line
			bp [bpEnd-1].flag = BLOCKED;

		appendBPList ( currentPixel.x, currentPixel.y, BLOCKED );
	}
	else
	if ( prevD == 0 )			// previously moving right
	{
		bp [bpEnd-1].flag = BLOCKED;	// block rightmost in line

		if ( d == 7 )			// if line started from above
			appendBPList ( currentPixel.x, currentPixel.y, BLOCKED );
		else
			appendBPList ( currentPixel.x, currentPixel.y, UNBLOCKED );
	}
	else					// prev. movung in some vert. dir.
	{
			appendBPList ( currentPixel.x, currentPixel.y, UNBLOCKED );
						// add pixel twice if local min or max
			if ( ( ( d >= 1 ) && ( d <= 3 ) ) && ( ( prevD >= 5 ) && ( prevD <= 7 ) ) ||
			     ( ( d >= 5 ) && ( d <= 7 ) ) && ( ( prevD >= 1 ) && ( prevD <= 3 ) ) )
				appendBPList ( currentPixel.x, currentPixel.y, UNBLOCKED );
	}
}

addBPList ( int d )
{
	if ( d == prevD )
		sameDirection ();
	else
	{
		differentDirection ( d );
		prevV = prevD;		 // new previous vertical direction
	}

	prevD = d;			 // new previous search direction
}

void	nextXY ( int& x, int& y, int direction )
{
	switch ( direction )		// 3 2 1
	{				// 4   0
		case 1:			// 5 6 7
		case 2:
		case 3:
			y--;		// go up
			break;

		case 5:
		case 6:
		case 7:
			y++;    	// go down
			break;
	}

	switch ( direction )
	{
		case 3:
		case 4:
		case 5:
			x--;		// go left
			break;

		case 1:
		case 0:
		case 7:
			x++;		// go right
			break;
	}
}

int	findBP ( int d )
{
	int	x = currentPixel.x;
	int	y = currentPixel.y;

	nextXY ( x, y, d );		// get x,y of pixel in direction d

	if ( getpixel ( x, y ) == borderColor )
	{
		addBPList ( d );	// add pixel x,y to the table
		currentPixel.x = x;	// pixel at x,y becomes current pixel
		currentPixel.y = y;

		return TRUE;
	}

	return FALSE;
}

int	findNextPixel ()
{
	for ( int i = -1; i <= 5; i++ )
	{
		int	flag = findBP ( ( D + i ) & 7 );// find next border pixel

		if ( flag )			// flag is TRUE if found
		{
			D = ( D + i ) & 6;	// (D+i) MOD 2

			return flag;
		}
	}
}

int	scanRight ( int x, int y )
{
	while ( getpixel ( x, y ) != borderColor )
		if ( ++x == 639 )
			break;

	return x;
}

void	scanRegion ( int& x, int& y )
{
	for ( int i = bpStart; i < bpEnd; )
	{
		if ( bp [i].flag == BLOCKED )	// skip pixel if blockeds
			i++;
		else
		if ( bp [i].y != bp [i+1].y )	// skip last pixel in line
			i++;
		else
		{				// if at least one pixel
						// to fill then scan the line
			if ( bp [i].x < bp [i+1].x - 1 )
			{
				int	xr = scanRight ( bp [i].x + 1, bp [i].y );

				if ( xr < bp [i+1].x )
				{
					x = xr;
					y = bp [i].y;
					break;
				}
			}

			i += 2;
		}
	}

	bpStart = i;
}

int	compareBP ( BPStruct * arg1, BPStruct * arg2 )
{
	int	i = arg1 -> y - arg2 -> y;

	if ( i != 0 )
		return i;

	if ( ( i = arg1 -> x - arg2 -> x ) != 0 )
		return i;

	return arg1 -> flag - arg2 -> flag;
}

void	sortBP ()
{
	qsort ( bp + bpStart, bpEnd - bpStart, sizeof ( BPStruct ),
		(int (*)(const void *, const void *)) compareBP );
}

void	fillRegion ()
{
	for ( int i = 0; i < bpEnd; )
	{
		if ( bp [i].flag == BLOCKED )	// skip pixel if blocked
			i++;
		else
		if ( bp [i].y != bp [i+1].y )	// skip pixel if last in line
			i++;
		else				// if at least one pixel to fill
		{				// draw a line
			if ( bp [i].x < bp [i+1].x - 1 )
				line ( bp [i].x + 1, bp [i].y, bp [i+1].x - 1, bp [i+1].y );

			i += 2;
		}
	}
}

void	traceBorder ( int startX, int startY )
{
	int	nextFound;
	int	done;

	currentPixel.x = startX;
	currentPixel.y = startY;
	D              = 6;		// current search direction
	prevD          = 8;		// previous search direction
	prevV          = 2;		// most recent vertical direction

	do				// loop around the border
	{				// until returned to starting pixel
		nextFound = findNextPixel ();
		done      = (currentPixel.x == startX) && (currentPixel.y == startY);
	} while ( nextFound && !done );

	if ( !nextFound )		// if only one pixel in border
	{				// add it twice to the table
		appendBPList ( startX, startY, UNBLOCKED );
		appendBPList ( startX, startY, UNBLOCKED );
	}
	else				// if last search direction was upward
					// add the starting pixel to the table
	if ( (prevD <= 3) && (prevD >= 1) )
		appendBPList ( startX, startY, UNBLOCKED );
}

borderFill ( int x, int y )
{
	do				// do until entiretable is skanned
	{
		traceBorder ( x, y );	// trace border startint at x,y
		sortBP      ();		// sort border pixel table
		scanRegion  ( x, y );	// look for holes in the interior
	} while ( bpStart < bpEnd );

	fillRegion ();			// use the table to fill the interior
}

/////////////////////////////////////////////////////////////////////////////

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	initgraph ( &driver, &mode, "" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	circle ( 320, 200, 140 );
	circle ( 260, 200, 40 );
	circle ( 380, 200, 40 );

	getch ();

	setcolor ( fillColor );

	borderFill ( 460, 200 );	// give seed point on right border

	getch ();
	closegraph ();
}
