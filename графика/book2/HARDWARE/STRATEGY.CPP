#include	"mouse.h"
#include	"array.h"
#include	"video.h"
#include	"image.h"
#include	"sprite.h"

char	screenMap [MAX_TILE_X][MAX_TILE_Y];
Image * tiles [MAX_TILES];
Array * sprites;
Sprite  mouse;
int	orgX = 0;
int	orgY = 0;
int	done = 0;

void	drawScreen ()
{
	int	i0 = orgX / tileWidth;
	int	i1 = (orgX + screenWidth - 1) / tileWidth;
	int	x0 = i0 * tileWidth - orgX;
	int	j0 = orgY / tileHeight;
	int	j1 = (orgY + screenHeight - 1) / tileHeight;
	int	y0 = j0 * tileHeight - orgY;

	for ( int i = i0, x = x0; i <= i1; i++, x += tileWidth )
		for ( int j = j0, y = y0; j <= j1; j++, y += tileHeight )
			tiles [screenMap [i][j]] -> draw ( x, y );

	for ( i = 0; i < sprites -> getCount (); i++ )
		( (Sprite *) sprites -> objectAt ( i ) ) -> draw ();

	mouse.draw ();

	swapBuffers ();
}

main ()
{
	MouseState mouseState;

	loadMap     ();
	loadTiles   ();
	loadSprites ();
	initVideo   ();
	resetMouse  ();


	for ( ; !done; )
	{
		performActions ();
		drawScreen     ();
		readMouseState ( mouseState );

		if ( mouseState.loc.x >= screenWidth - 2 && orgX < maxOrgX )
			orgX++;
		else
		if ( mouseState.loc.x <= 2 && orgX > 0 )
			orgX--;

		if ( mouseState.loc.y >= screenHeight - 2 && orgY < maxOrgY )
			orgY++;
		else
		if ( mouseState.loc.y <= 2 && orgY > 0 )
			orgY--;

		mouse.set ( mouseState.loc.x + orgX, mouseState.loc.y + orgY );

		if ( mouseState.buttons )
			handleMouse ( mouseState );

		handleKeyboard ();
	}

	doneVideo   ();
	freeTiles   ();
	freeSprites ();
}