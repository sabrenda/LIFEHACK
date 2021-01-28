#include	<stdio.h>
#include	"rasfont.h"
#include	"store.h"

main ()
{
	File    fontFile ( "sserif10.rfn" );
	Font  * systemFont = new RasterFont ( &fontFile );

	int	ch = 'q';

	int     charWidth  = systemFont -> charWidth ( ch );
	int     charHeight = systemFont -> height;

	char  * image             = (char *) systemFont -> charImage ( ch );
	int     imageBytesPerLine = (charWidth+7) >> 3;

        printf ( "\n---------------------------\n" );

	for ( register int i = 0; i < charHeight; i++ )
	{
		for ( register int j = 0; j < charWidth; j++ )
			if ( image [j>>3] & ( 0x80 >> (j & 7) ) )
				printf ( "x" );
			else
				printf ( " " );

		image += imageBytesPerLine;
		printf ( "\n" );
	}

	printf ( "\n---------------------------\n" );
}
