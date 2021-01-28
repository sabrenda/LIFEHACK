#include	<alloc.h>
#include	<dos.h>
#include	"sprite.h"

Sprite :: Sprite ( int w, int h, char * im1, ... )
{
	char ** imPtr = &im1;

	x          = 0;
	y          = 0;
	width      = w;
	height     = h;
	curStage   = 0;
	underImage = (char *) malloc ( width * height );

	for ( stageCount = 0; * imPtr != NULL; imPtr ++, stageCount ++ )
		image [stageCount] = * imPtr;
}

void	Sprite :: draw ()
{
	int	   x1 = max ( 0, x - orgX );
	int	   x2 = min ( screenWidth, x - orgX + width );

	if ( x2 < x1 )
		return;

	int	   y1 = max ( 0, y - orgY );
	int	   y2 = min ( screenHeight, y - orgY + height );

	if ( y2 < y1 )
		return;

	char far * videoPtr = videoAddr + y1 * screenWidth + x1;
	char	 * dataPtr  = image [curStage] + (y1 - y)*width + (x1 - x);
	int	   step     = screenWidth - (x2 - x1);

	for ( register int y = y1; y < y2; y++, videoPtr += step )
	{
		for ( int x = x1; x < x2; x++, dataPtr++, videoPtr++ )
			if ( * dataPtr != TRANSP_COLOR )
				* videoPtr = * dataPtr;

		dataPtr += width - (x2 - x1);
	}
}

void	Sprite :: storeUnder ()
{
	int	   x1 = max ( 0, x - orgX );
	int	   x2 = min ( screenWidth, x - orgX + width );
	int	   y1 = max ( 0, y - orgY );
	int	   y2 = min ( screenHeight, y - orgY + height );
	char far * videoPtr = videoAddr + y1 * screenWidth + x1;
	char	 * ptr = underImage;
	int	   step     = screenWidth - (x2 - x1);

	for ( register int y = y1; y < y2; y++, videoPtr += step )
		for ( register int x = x1; x < x2; x++ )
			* ptr ++ = * videoPtr ++;
}

void	Sprite :: restoreUnder ()
{
	int	   x1 = max ( 0, x - orgX );
	int	   x2 = min ( screenWidth, x - orgX + width );
	int	   y1 = max ( 0, y - orgY );
	int	   y2 = min ( screenHeight, y - orgY + height );
	char far * videoPtr = videoAddr + y1 * screenWidth + x1;
	char	 * ptr = underImage;
	int	   step     = screenWidth - (x2 - x1);

	for ( register int y = y1; y < y2; y++, videoPtr += step )
		for ( register int x = x1; x < x2; x++ )
			* videoPtr ++ = * ptr ++;
}

