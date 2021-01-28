#include        <malloc.h>
#include        <math.h>
#include        "FixMath.h"

Fixed * sinTable;
Fixed * cosTable;
Fixed * tanTable;
Fixed * cotanTable;
Fixed * invSinTable;
Fixed * invCosTable;

void    initFixMath ()
{
        sinTable    = new Fixed [8192];
        cosTable    = new Fixed [8192];
        tanTable    = new Fixed [8192];
        cotanTable  = new Fixed [8192];
        invSinTable = new Fixed [8192];
        invCosTable = new Fixed [8192];

        for ( int i = 0; i < 8192; i++ )
        {
                float   x  = i * 2 * M_PI / (8192.0);
                float   sx = sin ( x );
                float   cx = cos ( x );
                float   tx = tan ( x );

                sinTable [i] = float2Fixed ( sx );
                cosTable [i] = float2Fixed ( cx );
                tanTable [i] = float2Fixed ( tx );

                if ( tx > 0.0001 || tx < -0.0001 )
                        cotanTable [i] = (long)( 65536.0 / tx );
                else
                        cotanTable [i] = ( tx > 0 ? 65536.0 * 10000.0 : -65536.0 * 10000.0 );

                if ( sx > 0.0001 || sx < -0.0001 )
                        invSinTable [i] = (long)( 65536.0 / sx );
                else
                        invSinTable [i] = ( sx > 0 ? 65536.0 * 10000.0 : -65536.0 * 10000.0 );

                if ( cx > 0.0001 || cx < -0.0001 )
                        invCosTable [i] = (long)( 65536.0 / cx );
                else
                        invCosTable [i] = ( cx > 0 ? 65536.0 * 10000.0 : -65536.0 * 10000.0 );
        }
}
