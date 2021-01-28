//
// class ImagePyramid realizes mip-mapping
// for a  given image
//
// Author: Alex V. Boreskoff
//
#ifndef	__PYRAMID__
#define	__PYRAMID__

#include	"bmp.h"

#define	MAX_LEVELS	10		// maximum # of levels in pyramid

class	ImagePyramid
{
public:
	char  * data;
	RGB   * palette;
	int	palSize;
	int	width;
	int	offs [MAX_LEVELS];

	ImagePyramid ( BMPImage * );
	~ImagePyramid ();
};

int	findClosestEntry ( int, int, int, RGB *, int );
#endif
