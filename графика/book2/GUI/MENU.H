//
// Basic class for menus (supproting images in menu items)
// Author: Alex V. Boreskoff
//
#ifndef	__MENU__
#define	__MENU__

#include	"array.h"
#include	"win.h"

#define	MIF_ENABLED	0x0001		// item is enabled
#define	MIF_HASSUBMENU	0x0002		// item has submenu attached

#define	MIS_ACTIVE	0x0001

struct MenuItem
{
	char	  text [80];
	Surface * pic;
	int       flags;
	Menu    * subMenu;	// submenu or id of command
};

class	Menu : public Window
{
public:
	Array	items;
	int	activeItem;
	int	indent;			// indent for text (due to pics)

	Menu ( int x, int y, char * caption ) : Window ( x, y, 0, 0, caption, deskTop ), items ( 10, 10 )
	{
		style      = WS_MOVEABLE | WS_FLOATING;
		status    &= ~WS_ENABLED;
		activeItem = -1;
	}
	~Menu ();

	virtual	char  * getClassName () const
	{
		return "Menu";
	}

	virtual int     put ( Store * ) const; // save object to the store
	virtual	int	get ( Store * );       // load object from the store

	virtual	void	drawItem  ( int x, int y, MenuItem * item, int itemStatus ) const;
	virtual	void	show      ();
	virtual	int	mouseDown ( const Message& );
	virtual	int	mouseMove ( const Message& );
	virtual	int	mouseUp   ( const Message& );
	virtual	void	draw      ( const Rect& ) const;

	int		addItem   ( int id, char * itemText, Surface * pic = NULL );
	int		addItem   ( Menu * subMenu, char * itemText, Surface * pic = NULL );
	void		calcSize  ();

};
#endif


