#include	<alloc.h>
#include	<dos.h>
#include	"sprite2.h"

Sprite :: Sprite ( int w, int h, char * im1, ... )
{
	char ** imPtr = &im1;

	x          = 0;
	y          = 0;
	width      = w;
	height     = h;
	curStage   = 0;
	underImage = (char *) malloc ( width * height );

	for ( int lineCount = 0; * imPtr != NULL; imPtr ++ )
	{
		char  * ptr = * imPtr;

		for ( int i = 0; i < height; i++ )
		{
			lineStart [lineCount] = ptr;

			for ( int j = 0; j < width; j++ )
			{
				int	count = * ptr++;

				if ( ( count & 0x80 ) == 0 )	// not transparent
					ptr += count & 0x7F;

				j += count & 0x7F;
			}
		}
	}
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
	int	   step     = screenWidth - (x2 - x1);

	for ( register int y = y1; y < y2; y++, videoPtr += step )
	{
		char      * dataPtr  = lineStart [curStage * height + y1 - y];
		char far  * videoPtr = videoAddr + y * screenWidth + x1;

		for ( int i = x; x < x1; )	// skip invisible pixels at start
		{
			int	count = * dataPtr++;

			if ( count & 0x80 )	// transparent block
				i += count & 0x7F;
			else
			{
				count &= 0x7F;

				if ( i + count < x1 )
				{
					i       += count;
					dataPtr += count;
				}
				else
				{
					dataPtr += x1 - i;
					count   -= x1 - i;

					if ( count > x2 - i )
						count = x2 - i;

					for ( ; count > 0; count-- )
						* videoPtr++ = * dataPtr++;
				}
			}

		}

		for ( ; i < x2; i++ )
		{
			int	count = * dataPtr++;

			if ( count & 0x80 )
			{
				count    &= 0x7F;
				i        += count;
				videoPtr += count;
			}
			else
			{
				count &= 0x7F;

				if ( count > x2 - i )
					count = x2 - i;

				i += count;

				for ( ; count > 0; count-- )
					* videoPtr++ = * dataPtr++;
			}
		}
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
