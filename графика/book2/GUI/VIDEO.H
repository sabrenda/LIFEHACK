//
// Basic video functions & constants
// Author: Alex V. Boreskoff
//
#ifndef	__VIDEO__
#define	__VIDEO__

#include	"surface.h"
#include	"resfile.h"

#define BLACK           palette [0]
#define BLUE            palette [1]
#define GREEN           palette [2]
#define CYAN            palette [3]
#define RED             palette [4]
#define MAGENTA         palette [5]
#define BROWN           palette [6]
#define LIGHTGRAY       palette [7]
#define DARKGRAY        palette [8]
#define LIGHTBLUE       palette [9]
#define LIGHTGREEN      palette [10]
#define LIGHTCYAN       palette [11]
#define LIGHTRED        palette [12]
#define LIGHTMAGENTA    palette [13]
#define YELLOW          palette [14]
#define WHITE           palette [15]

extern	ResFile * systemResource;
extern	Surface * screenSurface;
extern	int     * palette;
extern	Rect	  screenRect;

int	  initVideo ( int xRes, int yRes, int bpp );
int	  doneVideo ();
Surface * loadBMPFromResource  ( ResFile *, const char * resName );
Font    * loadFontFromResource ( ResFile *, const char * resName );
#endif
