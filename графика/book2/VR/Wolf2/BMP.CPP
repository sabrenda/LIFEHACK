#include	<mem.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<io.h>

#include	"Bmp.h"

#define	BI_RGB	0l
#define	BI_RLE8	1l
#define	BI_RLE4	2l

struct	BMPHeader
{
	int	Type;			// type of file, must be 'BM'
	long	Size;			// size of file in bytes
	int	Reserved1, Reserved2;
	long	OffBits;		// offset from this header to actual data
};

struct	BMPInfoHeader
{
	long	Size;			//
	long	Width;			// width of bitmap in pixels
	long	Height;			// height of bitmap in pixels
	int	Planes;			// # of planes
	int	BitCount;		// bits per pixel
	long	Compression;		// type of compression, BI_RGB - no compression
	long	SizeImage;		// size of image in bytes
	long	XPelsPerMeter;		// hor. resolution of the target device
	long	YPelsPerMeter;		// vert. resolution
	long	ClrUsed;
	long	ClrImportant;
};

struct	RGBQuad
{
	char	Blue;
	char	Green;
	char	Red;
	char	Reserved;
};

BMPImage :: BMPImage ( char * FileName )
{
	int		file = open ( FileName, O_RDONLY | O_BINARY );
	BMPHeader	Hdr;
	BMPInfoHeader	InfoHdr;
	RGBQuad		Pal [256];

	Palette = NULL;			// no data yet
	Data    = NULL;

	if ( file == -1 )		// cannot open
		return;
					// read header data
	read ( file, &Hdr,     sizeof ( Hdr ) );
	read ( file, &InfoHdr, sizeof ( InfoHdr ) );

	int	 NumColors = 1 << InfoHdr.BitCount;
	unsigned NumBytes  = (unsigned) filelength ( file ) - Hdr.OffBits;
	int	 x, y;
	int	 Count;
	int	 Shift = ( 4 - ( InfoHdr.Width % 4 ) ) % 4;
	char   * buf   = (char *) malloc ( NumBytes );
	char   * ptr   = buf;

	if ( buf == NULL )
	{
		close ( file );
		return;
	}

	Width   = InfoHdr.Width;
	Height  = InfoHdr.Height;
	Palette = new RGB [NumColors];
	Data    = (char *) malloc ( (unsigned)Width * (unsigned)Height );

	if ( Data == NULL )
	{
		free ( buf );
		close ( file );
		return;
	}
					// prepare palettes
	read  ( file, Pal, NumColors * sizeof ( RGBQuad ) );

	for ( int i = 0; i < NumColors; i++ )
	{
		Palette [i].Red   = Pal [i].Red;
		Palette [i].Green = Pal [i].Green;
		Palette [i].Blue  = Pal [i].Blue;
	}
					// read raw data
	lseek ( file, Hdr.OffBits, SEEK_SET );
	read  ( file, buf, NumBytes );
	close ( file );


	memset ( Data, '\0', InfoHdr.Width*(unsigned)InfoHdr.Height );

	if ( InfoHdr.Compression == BI_RGB )
	{
		if ( InfoHdr.BitCount == 4 )	// 16-colors uncompressed
		{
			for ( y = Height - 1; y >= 0; y--, ptr += Shift )
				for ( x = 0; x < Width; x += 2, ptr++ )
				{
					Data [ y * Width + x    ] = (*ptr) >> 4;
					Data [ y * Width + x + 1] = (*ptr) & 0x0F;
				}
		}
		else
		if ( InfoHdr.BitCount == 8 )	// 256-colors uncompressed
		{
			for ( y = Height - 1; y >= 0; y--, ptr += Shift )
				for ( x = 0; x < Width; x++, ptr++ )
					Data [ y * Width + x ] = *ptr;
		}
	}

	free ( buf );
}

BMPImage :: ~BMPImage ()
{
	if ( Palette != NULL )
		delete Palette;

	if ( Data != NULL )
		free ( Data );
}

char	BMPImage :: GetPixel ( int x, int y )
{
	return Data [ x + y * Width ];
}

