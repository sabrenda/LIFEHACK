//
// Definitions for loading image files
// Author: Alex V. Boreskoff
//
#ifndef	__IMAGE_LOADER__
#define	__IMAGE_LOADER__

#include	"surface.h"

#define	LE_OK			0		// image loaded ok
#define	LE_WRONGFORMAT		1 		// inappropriate format for given loader
#define	LE_DATAERROR		2		// error in image data
#define	LE_INCOMPATIBLEFORMAT	3		// given format is incompatible
						// with file data

Surface * loadBMP  ( const void * data, int size, const Surface& surface, int& code );
Surface * loadGIF  ( const void * data, int size, const Surface& surface, int& code );
Surface * loadTGA  ( const void * data, int size, const Surface& surface, int& code );
Surface * loadPCX  ( const void * data, int size, const Surface& surface, int& code );
Surface * loadPNG  ( const void * data, int size, const Surface& surface, int& code );
Surface * loadJPEG ( const void * data, int size, const Surface& surface, int& code );

#endif
