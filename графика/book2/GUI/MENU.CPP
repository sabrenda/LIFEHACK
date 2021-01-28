//
// Basic class for menus (supporting images in menu items)
// Author: Alex V. Boreskoff
//
#include        "menu.h"
#include        "store.h"

Menu :: ~Menu ()
{
        for ( int i = 0; i < items.getCount (); i++ )
                delete (MenuItem *) items [i];
}

int     Menu :: put ( Store * s ) const
{
        if ( !View :: put ( s ) )
                return FALSE;

        int     itemCount = items.getCount ();

        s -> putInt ( itemCount );

        for ( int i = 0; i < itemCount; i++ )
        {
                MenuItem * item = (MenuItem *) items.at ( i );

                s -> putString ( item -> text );
                s -> putInt    ( item -> flags );
                s -> putInt    ( item -> pic != NULL );

                if ( item -> pic != NULL )
                        item -> pic -> put ( s );

                if ( item -> flags & MIF_HASSUBMENU )
                        item -> subMenu -> put ( s );
                else
                        s -> putInt ( (int) item -> subMenu );
        }

        return TRUE;
}

int     Menu :: get ( Store * s )
{
        if ( !View :: get ( s ) )
                return FALSE;

        int     itemsCount = s -> getInt ();

        for ( int i = 0; i < itemsCount; i++ )
        {
                char    * itemText    = s -> getString ();
                int       itemFlags   = s -> getInt    ();
                int       itemPic     = s -> getInt    ();
                Surface * pic         = NULL;
                Menu    * itemSubMenu = NULL;

                if ( itemPic )
                        pic = (Surface *) s -> getObject ();

                if ( itemFlags & MIF_HASSUBMENU )
                {
                        itemSubMenu = (Menu *) s -> getObject ();

                        addItem ( itemSubMenu, itemText, pic );
                }
                else
                        addItem ( s -> getInt (), itemText, pic );

                free ( itemText );
        }

        return TRUE;
}

int     Menu :: addItem ( int id, char * itemText, Surface * pic )
{
        MenuItem * item = new MenuItem;

        strcpy ( item -> text, itemText );
        item -> subMenu = (Menu *) id;
        item -> pic     = pic;
        item -> flags   = MIF_ENABLED;

        return items.insert ( item );
}

int     Menu :: addItem ( Menu * subMenu, char * itemText, Surface * pic )
{
        MenuItem * item = new MenuItem;

        strcpy ( item -> text, itemText );
        item -> subMenu = subMenu;
        item -> pic     = pic;
        item -> flags   = MIF_ENABLED | MIF_HASSUBMENU;

        return items.insert ( item );
}

void    Menu :: drawItem ( int x, int y, MenuItem * item, int itemStatus ) const
{
        int     w = width ();

        screenSurface -> setColor ( WHITE );
        screenSurface -> drawLine ( x + 1, y, x + w - 1, y );
        screenSurface -> drawLine ( x + 1, y, x + 1,     y + CAPTION_SIZE );
        screenSurface -> setColor ( DARKGRAY );
        screenSurface -> drawLine ( x + 2,     y + CAPTION_SIZE - 1, w + w - 2, y + CAPTION_SIZE - 1 );
        screenSurface -> drawLine ( x + w - 2, y + CAPTION_SIZE - 1, x + w - 2, y + 2 );
        screenSurface -> setColor ( BLACK );
        screenSurface -> drawLine ( x, y + CAPTION_SIZE, x + w - 1, y + CAPTION_SIZE );
        screenSurface -> setColor ( itemStatus & MIS_ACTIVE ? WHITE : LIGHTGRAY );
        screenSurface -> drawBar ( x + 2, y + 1, x + w - 2, y + CAPTION_SIZE - 2 );

        if ( item -> pic != NULL )
                item -> pic -> copy ( *screenSurface, Point ( x + 4, (CAPTION_SIZE - item->pic->height)/2 ) );

        screenSurface -> setFont    ( sysFont );
        screenSurface -> setColor   ( BLACK );
        screenSurface -> drawString ( x + indent, y + (CAPTION_SIZE - sysFont -> height)/2, item -> text );

        if ( item -> flags & MIF_HASSUBMENU )
        {
                screenSurface -> setColor ( BLACK );
                screenSurface -> drawLine ( x + w - 12, y + 7, x + w - 12, y + 13 );
                screenSurface -> setColor ( DARKGRAY );
                screenSurface -> drawLine ( x + w - 11,  y + 7, x + w - 4, y + 10 );
                screenSurface -> setColor ( itemStatus & MIS_ACTIVE ? LIGHTGRAY : WHITE );
                screenSurface -> drawLine ( x + w - 11, y + 13, x + w - 4, y + 10 );
        }
}

void    Menu :: calcSize ()
{
        int     tw, th;
        int     w;
        int     h = CAPTION_SIZE;

        sysFont -> size ( tw, th, text );

        w      = tw + 40;
        indent = 4;

        for ( int i = 0; i < items.getCount (); i++ )
        {
                MenuItem * item = (MenuItem *) items [i];

                if ( item -> flags & MIF_ENABLED )
                {
                        sysFont -> size ( tw, th, item -> text );

                        if ( item -> pic != NULL )
                        {
                                tw += 5 + item -> pic -> width;

                                if ( item -> pic -> width + 4 > indent )
                                        indent = item -> pic -> width + 4;
                        }

                        if ( tw + 10 > w )
                                w = tw + 10;

                        h += CAPTION_SIZE + 1;
                }
        }

        area.x2 = area.x1 + w - 1;
        area.y2 = area.y1 + h + 1;
}

void    Menu :: show ()
{
        calcSize ();

        Window :: show ();
}

void    Menu :: draw ( const Rect& r ) const
{
        Window :: draw ( r );

        for ( int i = 0, y = CAPTION_SIZE + 2; i < items.getCount (); i++ )
        {
                MenuItem * item = (MenuItem *) items.at ( i );

                if ( item -> flags & MIF_ENABLED == 0 )
                        continue;

                drawItem ( 0, y, item, i == activeItem ? MIS_ACTIVE : 0 );

                y += CAPTION_SIZE + 1;
        }
}


int     Menu :: mouseDown ( const Message& m )
{
        int     newActive = (m.mouse.loc.y - CAPTION_SIZE) / CAPTION_SIZE;

        if ( m.mouse.loc.y >= CAPTION_SIZE && newActive >= 0 && newActive < items.getCount () )
        {
                activeItem = newActive;

                captureMouse ( this );
                repaint ();
        }

        return Window :: mouseDown ( m );
}

int     Menu :: mouseMove ( const Message& m )
{
        if ( m.mouse.loc.x >= 0 && m.mouse.loc.y >= 0 &&
             m.mouse.loc.x < width () && m.mouse.loc.y < height () )
        {
                int     newActive = (m.mouse.loc.y - CAPTION_SIZE) / CAPTION_SIZE;

                if ( newActive >= 0 && newActive < items.getCount () && m.mouse.buttons & MK_LEFT )
                        if ( newActive != activeItem )
                        {
                                activeItem = newActive;

                                repaint ();
                        }
        }

        return Window :: mouseMove ( m );
}

int     Menu :: mouseUp ( const Message& m )
{
        releaseMouse ();

        if ( m.mouse.loc.x >= 0 && m.mouse.loc.y >= 0 &&
             m.mouse.loc.x < width () && m.mouse.loc.y < height () )
                if ( activeItem >= 0 && activeItem < items.getCount () )
                {
                        MenuItem * item = (MenuItem *) items [activeItem];

                        if ( item -> flags & MIF_HASSUBMENU && item [activeItem].subMenu != NULL )
                        {
                                int     x   = area.x2;
                                int     y   = area.y1 + activeItem*CAPTION_SIZE;
                                Menu  * sub = item -> subMenu;

                                sub -> calcSize ();
                                sub -> style |= WS_HASCLOSEBOX;

                                if ( x + sub -> width () >= screenRect.width () )
                                        x = area.x1 - sub -> width () - 1;

                                sub -> area.move ( x - sub -> area.x1, y - sub -> area.y1 );
                                sub -> show ();
                        }
                        else
                                sendMessage ( delegate, WM_COMMAND, 0, (int) item -> subMenu, this );
                }

        activeItem = -1;

        if ( style & WS_HASCLOSEBOX )
                hide ();
        else
                repaint ();

        return Window :: mouseUp ( m );
}
