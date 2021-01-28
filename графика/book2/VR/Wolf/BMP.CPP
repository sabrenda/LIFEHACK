#include	<mem.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<io.h>

#include	"bmp.h"

#define	BI_RGB	0l
#define	BI_RLE8	1l
#define	BI_RLE4	2l

struct	BMPHeader
{
	int	type;			// type of file, must be 'BM'
	long	size;			// size of file in bytes
	int	reserved1, reserved2;
	long	offBits;		// offset from this header to actual data
};

struct	BMPInfoHeader
{
	long	size;			//
	long	width;			// width of bitmap in pixels
	long	height;			// height of bitmap in pixels
	int	planes;			// # of planes
	int	bitCount;		// bits per pixel
	long	compression;		// type of compression, BI_RGB - no compression
	long	sizeImage;		// size of image in bytes
	long	xPelsPerMeter;		// hor. resolution of the target device
	long	yPelsPerMeter;		// vert. resolution
	long	clrUsed;
	long	clrImportant;
};

struct	RGBQuad
{
	char	blue;
	char	green;
	char	red;
	char	reserved;
};

BMPImage :: BMPImage ( char * fileName )
{
	int		file = open ( fileName, O_RDONLY | O_BINARY );
	BMPHeader	hdr;
	BMPInfoHeader	infoHdr;
	RGBQuad		pal [256];

	palette = NULL;			// no data yet
	data    = NULL;

	if ( file == -1 )		// cannot open
		return;
					// read header data
	read ( file, &hdr,     sizeof ( hdr ) );
	read ( file, &infoHdr, sizeof ( infoHdr ) );

	int	 numColors = 1 << infoHdr.bitCount;
	unsigned numBytes  = (unsigned) filelength ( file ) - hdr.offBits;
	int	 x, y;
	int	 count;
	int	 shift = ( 4 - ( infoHdr.width % 4 ) ) % 4;
	char   * buf   = (char *) malloc ( numBytes );
	char   * ptr   = buf;

	if ( buf == NULL )
	{
		close ( file );

		return;
	}

	width   = infoHdr.width;
	height  = infoHdr.height;
	palette = new RGB [numColors];
	data    = (char *) malloc ( (unsigned)width * (unsigned)height );

	if ( data == NULL )
	{
		free  ( buf );
		close ( file );

		return;
	}
					// prepare palettes
	read  ( file, pal, numColors * sizeof ( RGBQuad ) );

	for ( int i = 0; i < numColors; i++ )
	{
		palette [i].red   = pal [i].red;
		palette [i].green = pal [i].green;
		palette [i].blue  = pal [i].blue;
	}
					// read raw data
	lseek ( file, hdr.offBits, SEEK_SET );
	read  ( file, buf, numBytes );
	close ( file );


	memset ( data, '\0', infoHdr.width*(unsigned)infoHdr.height );

	if ( infoHdr.compression == BI_RGB )
	{
		if ( infoHdr.bitCount == 4 )	// 16-colors uncompressed
		{
			for ( y = height - 1; y >= 0; y--, ptr += shift )
				for ( x = 0; x < width; x += 2, ptr++ )
				{
					data [ y * width + x    ] = (*ptr) >> 4;
					data [ y * width + x + 1] = (*ptr) & 0x0F;
				}
		}
		else
		if ( infoHdr.bitCount == 8 )	// 256-colors uncompressed
		{
			for ( y = height - 1; y >= 0; y--, ptr += shift )
				for ( x = 0; x < width; x++, ptr++ )
					data [ y * width + x ] = *ptr;
		}
	}
	else					// unsupported compression models
	{
		free ( data );
		delete palette;

		data    = NULL;
		palette = NULL;
		width   = 0;
		height  = 0;
	}

	free ( buf );
}

BMPImage :: ~BMPImage ()
{
	if ( palette != NULL )
		delete palette;

	if ( data != NULL )
		free ( data );
}

