#include        <stdio.h>
#include        <stdarg.h>
#include        <stdlib.h>

#include        "vesasurf.h"
#include        "video.h"
#include        "mouse.h"
#include        "keyboard.h"
#include        "timer.h"
#include        "win.h"
#include        "menu.h"
#include        "static.h"
#include        "button.h"
#include        "radio.h"

class   ShowVESAInfoDialog : public DialogWindow
{
public:
        ShowVESAInfoDialog ();
};

ShowVESAInfoDialog :: ShowVESAInfoDialog () : DialogWindow ( 150, 150, 320, 180, "Card Info", deskTop )
{
        VESASurface * surf = (VESASurface *) screenSurface;

        new StaticText  ( 11,  30, 50, 20,  this, "OEM",      BLACK, LIGHTGRAY, SS_GRAY );
        new StaticText  ( 11,  60, 50, 20,  this, "Vendor",   BLACK, LIGHTGRAY, SS_GRAY );
        new StaticText  ( 11,  90, 50, 20,  this, "Product",  BLACK, LIGHTGRAY, SS_GRAY );

        new StaticText  ( 65,  30, 230, 20,  this, surf -> cardInfo -> OEM,             BLACK, WHITE, SS_3D );
        new StaticText  ( 65,  60, 230, 20,  this, surf -> cardInfo -> OEMVendorName,   BLACK, WHITE, SS_3D );
	new StaticText  ( 65,  90, 230, 20,  this, surf -> cardInfo -> OEMProductName,  BLACK, WHITE, SS_3D );

        new TextButton ( ( 320-60)/2, 140, 60, 20, this, IDOK, "Ok" );

        setStyle ( getStyle () & ~WS_RESIZEABLE );
        setDefID ( IDOK );
}

////////////////////////////////////////////////////////////////////////

void    okBox ( char * caption, char * msg ... )
{
	char    * buf = new char [1024];
        int       width, height;
        va_list  arg;

        va_start ( arg, msg );
        vsprintf ( buf, msg, arg );
        va_end   ( arg );

        sysFont -> size ( width, height, buf );

        if ( width < 150 )
                width = 150;

        DialogWindow    * dlg = new DialogWindow ( 200, 100, 100 + width, 100 + height, caption, deskTop );

        new StaticPic  ( 10, 30, dlg, loadBMPFromResource ( systemResource, "EXCLAM" ) );
	new TextButton ( ( 100+width-60)/2, 70 + height, 60, 20, dlg, IDOK, "Ok" );

        StaticText * text = new StaticText ( 70, 30, width + 10, height + 10, dlg, "", DARKGRAY, WHITE, SS_3D );

        text -> setText  ( buf );
        text -> setStyle ( text -> getStyle () | TS_CENTER );

        dlg -> setStyle ( dlg -> getStyle () & ~WS_RESIZEABLE );
        dlg -> setDefID ( IDOK );
        dlg -> execute  ();

        delete buf;
        delete dlg;
}

main ( int argc, char * argv [] )
{
        int     xRes = 800;
        int     yRes = 600;
	int     bpp  = 8;

        if ( !strcmp ( argv [1], "?" ) || !stricmp ( argv [1], "/?" ) || !stricmp ( argv [1], "-?" ) )
	{
                printf ( "\nGUI Sample application." );
                printf ( "\nUsage:\n\tgui xResolution yResolution bitsPerPixel" );
		printf ( "\nwhere (xResolution,yResoltion) is one of the following:" );
                printf ( "\n\t640x480" );
                printf ( "\n\t800x600" );
                printf ( "\n\t1024x768" );
                printf ( "\n\t1280x1024" );
                printf ( "\n\nand bitsPerPixel is one of the following:" );
                printf ( "\n\t8, 15, 16" );

                return 0;
        }

        if ( argc == 4 )
        {
		xRes = atoi ( argv [1] );
                yRes = atoi ( argv [2] );
                bpp  = atoi ( argv [3] );
	}

        if ( !initVideo ( xRes, yRes, bpp ) )
	{
                printf ( "\n%s.", ((VESASurface *) screenSurface) -> getStatusDescription () );

                return 1;
        }

	initKeyboard ();
	initTimer ();
        initMouse ();
        initWindows ();

        DeskTop * d  = new DeskTop;
        Window  * w1 = new Window ( 10,  10,  420, 220, "Window 1", d );
	Window  * w2 = new Window ( 110, 110, 300, 150, "Window 2", d );
        Window  * w3 = new Window ( 210, 210, 300, 200, "Window 3", d );
        Menu    * menu = new Menu ( 0, 0, "Main Menu" );

        new StaticText  ( 11,  30,  100, 20,  w1, "Hi, Guys !",  WHITE, DARKGRAY, SS_3D );
        new StaticText  ( 11,  60,  100, 20,  w1, "Hi, Guys !",  BLACK, WHITE, SS_3D );
	new StaticText  ( 320, 45,  77,  20,  w1, "Down Text",   BLACK, LIGHTGRAY, SS_DOWN );
        new StaticText  ( 320, 70,  77,  20,  w1, "Up Text",     BLACK, LIGHTGRAY, SS_UP );
        new StaticText  ( 320, 95,  77,  20,  w1, "Framed Text", BLACK, LIGHTGRAY,  SS_FRAMED );
        new StaticText  ( 320, 120, 77,  20,  w1, "Framed Text", BLACK, LIGHTGRAY,  SS_FRAMERAISED );

        new RadioGroup  ( 20,  140, 2,   1,   70, 20, 1001, 0, "Item1\nItem2", w1 );

        new TextButton ( 10, 30, 80, 20, w2, 1, "Button 1" );
        new TextButton ( 10, 60, 80, 20, w2, 2, "Button 2" );
        new TextButton ( 10, 90, 80, 20, w2, 3, "Button 3" );

        menu -> addItem ( IDOPEN,  "Open..." );
        menu -> addItem ( IDCLOSE, "Close" );
	menu -> addItem ( 10,      "Print..." );
        menu -> addItem ( IDQUIT,  "Exit" );

	menu -> show ();

        w1 -> show ();
	w2 -> show ();
        w3 -> show ();

        ShowVESAInfoDialog dlg;

        dlg.execute ();

        d  -> run ();

        okBox ( "OkBoxCaption", "Some text\n(multiline)" );

        doneKeyboard ();
        doneTimer ();
	doneWindows ();
        doneMouse ();
        doneVideo ();

        return 0;
}
