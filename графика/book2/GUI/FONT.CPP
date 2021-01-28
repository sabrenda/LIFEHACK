//
// Abstract font class
// Author : Alex V. Boreskoff
//

#include	"font.h"

void	Font :: size ( int& w, int& h, const char * str ) const
{
	int	ch;
	int	tw;

	for ( w = 0, tw = 0, h = height; * str != '\0'; str ++ )
		if ( * str == '\n' )
		{
			h += height;

			if ( tw > w )
				w = tw;
		}
		else
		{
			int	ch = * str;

			ch  = ( ch >= firstChar && ch <= lastChar ? ch - firstChar : '\0' );
			tw += charData [ch].width;
		}

	if ( tw > w )
		w = tw;
}
