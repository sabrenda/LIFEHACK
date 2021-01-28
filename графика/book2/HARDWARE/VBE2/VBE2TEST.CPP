#include        "vesa.h"
#include        <conio.h>
#include        <stdio.h>

void    writePixel ( int x, int y, int color )
{
        * ( x + y * bytesPerLine + (char *)lfbPtr ) = (char) color;
}

main ()
{
        if ( !initVBE2 () )
        {
                printf ( "\nVBE2 not found." );

                return 1;
        }

        printf ( "\nSign:%s",              vbeInfoPtr -> vbeSign );
        printf ( "\nVersion %04x",         vbeInfoPtr -> version );
        printf ( "\nOEM:%s",               vbeInfoPtr -> OEM );
        printf ( "\nOEMVendorName: %s",    vbeInfoPtr -> OEMVendorName );
        printf ( "\nOEMProductName: %s",   vbeInfoPtr -> OEMProductName );
        printf ( "\nOEMProductRev: %s",    vbeInfoPtr -> OEMProductRev );
        getch ();

        if ( !setVESAMode ( VESA_640x480x256 ) )
        {
                printf ( "\nError SetVESAMode." );

                return 1;
        }

        for ( int i = 0; i < 640; i++ )
                for ( int j = 0; j < 480; j++ )
                        writePixel ( i, j, (i/20 + 1)*(j/20 + 1) );

        getch ();

        doneVBE2 ();

        return 0;
}
