// test for VESA
#include	<conio.h>
#include	<dos.h>
#include	<stdio.h>
#include	<string.h>
#include	"Vesa.h"

int	findVESA ( VESAInfo& vi )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr vi
		mov	ax, 4F00h
		int	10h
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr vi
		mov	ax, 4F00h
		int	10h
		pop	di
	}
#endif
	if ( _AX != 0x004F )
		return 0;

	return !strncmp ( vi.sign, "VESA", 4 );
}

int	findVESAMode ( int mode, VESAModeInfo& mi )
{
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
	asm {
		push	es
		push	di
		les	di, dword ptr mi
		mov	ax, 4F01h
		mov	cx, mode
		int	10h
		pop	di
		pop	es
	}
#else
	asm {
		push	di
		mov	di, word ptr mi
		mov	ax, 4F01h
		mov	cx, mode
		int	10h
		pop	di
	}
#endif
	return _AX == 0x004F;

}

char * colorInfo ( int bits )
{
	switch ( bits )
	{
		case 4:
			return "16 colors";

		case 8:
			return "256 colors";

		case 15:
			return "32K colors ( HiColor )";

		case 16:
			return "64K colors ( HiColor )";

		case 24:
			return "16M colors ( TrueColor )";

		default:
			return "";
	}
}

void	dumpMode ( int mode )
{
	VESAModeInfo mi;

	if ( !findVESAMode ( mode, mi ) )
	{
		printf ( "\n%4X not found.", mode );
		return;
	}

	if ( ( mi.modeAttributes & 1 ) == 0 )	// not available now
	{
		printf ( "\n%4X not available.", mode );
		return;
	}

	printf ( "\n%4X %10s %4dx%4d %2d %s", mode,
			mi.modeAttributes & 0x10 ? "Graphics" : "Text",
			mi.xResolution, mi.yResolution, mi.bitsPerPixel,
			colorInfo ( mi.bitsPerPixel ) );
}

main ()
{
	VESAInfo info;
	char	 str [256];

	if ( !findVESA ( info ) )
	{
		printf ( "VESA VBE not found" );

		return 1;
	}

	_fstrcpy ( str, info.OEM );
	printf ( "\nVESA VBE version %d.%d\nOEM: %s\nTotal memory: %dKb\n",
		 info.version >> 8, info.version & 0xFF, str, info.totalMemory * 64 );

	for ( int i = 0; info.modeList [i] != -1; i++ )
		dumpMode ( info.modeList [i] );
}
