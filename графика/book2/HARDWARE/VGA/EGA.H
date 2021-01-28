#ifndef	__EGA__
#define	__EGA__

#include	<dos.h>

#define	EGA_GRAPHICS	0x3CE		// Graphics Controller base addr
#define	EGA_SEQUENCER	0x3C4		// Sequencer base addr
#define	EGA_CRTC	0x3D4

#define	EGA_SET_RESET		0
#define	EGA_ENABLE_SET_RESET    1
#define	EGA_COLOR_COMPARE	2
#define	EGA_DATA_ROTATE		3
#define	EGA_READ_MAP_SELECT	4
#define	EGA_MODE		5
#define	EGA_MISC		6
#define	EGA_COLOR_DONT_CARE	7
#define	EGA_BIT_MASK		8

#define	EGA_MAP_MASK		2

struct	RGB
{
	char	red;
	char	dreen;
	char	blue;
};

inline	void	writeReg ( int base, int reg, int value )
{
	outportb ( base,     reg );
	outportb ( base + 1, value );
}

inline	char	readReg ( int base, int reg )
{
	outportb ( base, reg );

	return inportb ( base + 1 );
}

inline	char	pixelMask ( int x )
{
	return 0x80 >> ( x & 7 );
}

inline	char	leftMask ( int x )
{
	return 0xFF >> ( x & 7 );
}

inline	char	rightMask ( int x )
{
	return 0xFF << ( 7 ^ ( x & 7 ) );
}

inline	void	setRWMode ( int readMode, int writeMode )
{
	writeReg ( EGA_GRAPHICS, EGA_MODE, ( writeMode & 3 ) | ( ( readMode & 1 ) << 3 ) );
}

inline	void	setWriteMode ( int mode )
{
	writeReg ( EGA_GRAPHICS, EGA_DATA_ROTATE, ( mode & 3 ) << 3 );
}

int	   findEGA ();
int	   findVGA ();
void	   setVideoMode ( int );
void	   setVisiblePage ( int );
char far * findROMFont ( int );
void	   setPalette ( RGB far * palette, int );

#endif