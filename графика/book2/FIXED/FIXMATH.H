#ifndef __FIXED_MATH__
#define __FIXED_MATH__

#include        <math.h>

#define ANGLE_90        16384U          // angle of 90 degrees
#define ANGLE_180       32768U          // angle of 180 degress
#define ANGLE_270       49152U          // angle of 270 degress
#define MAX_FIXED       0x7FFFFFFFl     // maximum possible Fixed number
#define ONE             0x10000l        // 1.0

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

typedef long            Fixed;
typedef unsigned short  Angle;

extern  Fixed * sinTable;
extern  Fixed * cosTable;
extern  Fixed * tanTable;
extern  Fixed * cotanTable;
extern  Fixed * invSinTable;
extern  Fixed * invCosTable;

inline  Fixed   int2Fixed ( int x )
{
        return ( (long) x ) << 16;
}

inline  Fixed   float2Fixed ( float x )
{
        return (long)( 65536.0 * x );
}

inline	int	fraction2Fixed ( int a, int b )
{
	return (long)( (((long) a) << 16) / (long) b );
}

inline  int     fixed2Int ( Fixed x )
{
        return (int) ( x >> 16 );
}

inline  float   fixed2Float ( Fixed x )
{
        return ((float)x) / 65536.0;
}

inline  Fixed   fixAbs ( Fixed x )
{
        return x > 0 ? x : -x;
}

inline  Fixed   frac ( Fixed x )
{
        return x & 0xFFFFl;
}

inline  Fixed   sine ( Angle angle )
{
        return sinTable [ angle >> 3 ];
}

inline  Fixed   cosine ( Angle angle )
{
        return cosTable [ angle >> 3 ];
}

inline  Fixed   tang ( Angle angle )
{
        return tanTable [ angle >> 3 ];
}

inline  Fixed   coTang ( Angle angle )
{
        return cotanTable [ angle >> 3 ];
}

inline  Fixed   invSine ( Angle angle )
{
        return invSinTable [ angle >> 3 ];
}

inline  Fixed   invCosine ( Angle angle )
{
        return invCosTable [ angle >> 3 ];
}

inline  Angle   rad2Angle ( float angle )
{
        return (Angle)(32768.0 * angle / M_PI);
}

inline  float   angle2Rad ( Angle angle )
{
        return ((float) angle) * M_PI / 32768.0;
}

void    initFixMath ();

#endif
