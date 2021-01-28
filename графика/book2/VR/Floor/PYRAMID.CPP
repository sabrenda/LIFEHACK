//
// class ImagePyramid realizes mip-mapping
// for a  given image
//
// Author: Alex V. Boreskoff
//
#include	<alloc.h>
#include	<mem.h>
#include	<stdlib.h>
#include	"pyramid.h"

ImagePyramid :: ImagePyramid ( BMPImage * im )
{
	width    = im -> width;
	palSize  = 256;
	data     = (char *) malloc ( ( 4 * width * width ) / 3 );
	palette  = new RGB [palSize];
	offs [0] = 0;
				// copy palette
	memcpy ( palette, im -> palette, palSize * sizeof ( RGB ) );

				// copy 0-th layer
	memcpy ( data, im -> data, width * width );

	for ( int level = 1, w = width; w > 0; level ++ )
	{
		char  * prev = data + offs [level - 1];
		char  * cur  = prev + w*w;

		offs [level] = offs [level - 1] + w*w;
		w           /= 2;

		for ( int i = 0; i < w; i++ )
			for ( int j = 0; j < w; j++ )
			{
				int	index = 2*i + 2*j*2*w;
				int	r, g, b;

				r = ( (int)palette [prev [index]].red + (int)palette [prev [index+1]].red +
				      (int)palette [prev [index+2*w]].red + (int)palette [prev [index+2*w+1]].red ) >> 2;

				g = ( (int)palette [prev [index]].green + (int)palette [prev [index+1]].green +
				      (int)palette [prev [index+2*w]].green + (int)palette [prev [index+2*w+1]].green ) >> 2;

				b = ( (int)palette [prev [index]].blue + (int)palette [prev [index+1]].blue +
				      (int)palette [prev [index+2*w]].blue + (int)palette [prev [index+2*w+1]].blue ) >> 2;

				cur [i+j*w] = findClosestEntry ( r, g, b, palette, palSize );
			}
	}
}

ImagePyramid :: ~ImagePyramid ()
{
	free ( data );
	delete palette;
}

int	findClosestEntry ( int r, int g, int b, RGB * palette, int palSize )
{
	int	minDist = 1024;
	int	index;

	for ( int i = 0; i < palSize; i++ )
	{
		int	d = abs ( r - palette [i].red ) + abs ( g - palette [i].green ) + abs ( b - palette [i].blue );

		if ( d < minDist )
		{
			minDist = d;
			index   = i;
		}
	}

	return index;
}
