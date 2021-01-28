//
// Class VESASurface for working with VESA SVGA modes
// Author: Alex V. Boreskoff
//
#ifdef  __WATCOMC__
#include        <i86.h>
#endif

#include        <dos.h>
#include        <malloc.h>
#include        <string.h>
#include        "vesasurf.h"

struct DPMIDosPtr
{
        unsigned short segment;
        unsigned short selector;
};

struct
{
        unsigned long   edi;
        unsigned long   esi;
	unsigned long   ebp;
        unsigned long   esp;
        unsigned long   ebx;
        unsigned long   edx;
        unsigned long   ecx;
        unsigned long   eax;
        unsigned short  flags;
        unsigned short  es, ds, fs, gs, ip, cs, sp, ss;
} rmRegs;

static  DPMIDosPtr     vbeInfoPool = { 0, 0 };
static  DPMIDosPtr     vbeModePool = { 0, 0 };
static  DPMIDosPtr     palPool     = { 0, 0 };
static  REGS           regs;
static  SREGS          sregs;

////////////////////////////////////////////////////////////////////////////

inline  void  * RM2PMPtr ( void * ptr )         // convert pointer from real
{                                               // mode ptr to protected mode ptr
	return (void *) ( ( ( ( (unsigned long) ptr ) >> 16 ) << 4 ) + ( ( (unsigned long) ptr ) & 0xFFFF ) );
}

void    DPMIAllocDosMem ( DPMIDosPtr& ptr, int paras )
{
        regs.w.ax = 0x0100;             // allocate DOS memory
        regs.w.bx = paras;              // # of memory in paragraphs

        int386 ( 0x31, &regs, &regs );

        ptr.segment  = regs.w.ax;       // real-mode segment of the block
        ptr.selector = regs.w.dx;       // selector for the allocated block
}

void    DPMIFreeDosMem ( DPMIDosPtr& ptr )
{
	regs.w.ax = 0x0101;             // free DOS memory
        regs.w.dx = ptr.selector;

        int386 ( 0x31, &regs, &regs );
}

void  * DPMIMapPhysical ( void * ptr, unsigned long size )
{
        regs.w.ax = 0x0800;             // map physical memory
        regs.w.bx = (unsigned short ) ( ( (unsigned long) ptr ) >> 16 );
        regs.w.cx = (unsigned short ) ( ( (unsigned long) ptr ) & 0xFFFF );
        regs.w.si = (unsigned short )( size >> 16 );
        regs.w.di = (unsigned short )( size & 0xFFFF );

        int386 ( 0x31, &regs, &regs );

        return (void *) ( ( regs.w.bx << 16 ) + regs.w.cx );
}

void    DPMIUnmapPhysical ( void * ptr )
{
        regs.w.ax = 0x0801;             // free physical address mapping
        regs.w.bx = (unsigned short ) ( ( (unsigned long) ptr ) >> 16 );
        regs.w.cx = (unsigned short ) ( ( (unsigned long) ptr ) & 0xFFFF );

        int386 ( 0x31, &regs, &regs );
}

void    RMVideoInt ()                   // execute real-mode video interrupt
{
        segread ( &sregs );
        
        regs.w.ax  = 0x0300;
	regs.w.bx  = 0x0010;
	regs.w.cx  = 0;
	regs.x.edi = FP_OFF ( &rmRegs );
	sregs.es   = FP_SEG ( &rmRegs );

	int386x ( 0x31, &regs, &regs, &sregs );
}

////////////////////////////// VESASurface methods //////////////////////////

VESASurface :: VESASurface ( int xRes, int yRes, int bpp )
{
	data   = NULL;
	width  = 0;
	height = 0;
	status = VSS_OK;

	if ( !initVBE2 () )
		return;

        if ( !setVESAMode ( xRes, yRes, bpp ) )
                return;
}

VESASurface :: ~VESASurface ()
{
        doneVBE2 ();

        data = NULL;            // so it won't be free'ed
}

int     VESASurface :: initVBE2 ()
{
        memset ( &regs,   '\0', sizeof ( regs ) );
        memset ( &sregs,  '\0', sizeof ( sregs ) );
        memset ( &rmRegs, '\0', sizeof ( rmRegs ) );

        DPMIAllocDosMem ( vbeInfoPool, 512 / 16 );
        DPMIAllocDosMem ( vbeModePool, 256 / 16 );
        DPMIAllocDosMem ( palPool,     3*256 / 16 );

        cardInfo = (VESAInfo *)( vbeInfoPool.segment * 16 );
        modeInfo = (VESAModeInfo *)( vbeModePool.segment * 16 );

        memset  ( cardInfo, '\0', sizeof ( VESAInfo ) );
        strncpy ( cardInfo -> vbeSign, "VBE2", 4 );

        rmRegs.eax = 0x4F00;
        rmRegs.es  = vbeInfoPool.segment;
        rmRegs.edi = 0;

        RMVideoInt ();

	if ( rmRegs.eax != 0x4F )
	{
		status = VSS_VBENOTFOUND;

		return 0;
	}

	cardInfo -> OEM            = (char *)  RM2PMPtr ( cardInfo -> OEM );
	cardInfo -> modeListPtr    = (short *) RM2PMPtr ( cardInfo -> modeListPtr );
	cardInfo -> OEMVendorName  = (char *)  RM2PMPtr ( cardInfo -> OEMVendorName );
	cardInfo -> OEMProductName = (char *)  RM2PMPtr ( cardInfo -> OEMProductName );
	cardInfo -> OEMProductRev  = (char *)  RM2PMPtr ( cardInfo -> OEMProductRev );

	if ( strncmp ( cardInfo -> vbeSign, "VESA", 4 ) )
	{
		status = VSS_WRONGSIGN;

		return 0;
	}

	if ( cardInfo -> version >= 0x0200 && cardInfo -> OEMVendorName == NULL )
		cardInfo -> version = 0x0102;

	if ( cardInfo -> version < 0x200 )
		status = VSS_WRONGVERSION;

	return cardInfo -> version >= 0x200;
}

void    VESASurface :: doneVBE2 ()
{
	if ( data != NULL )                     // unmap mapped LFB
		DPMIUnmapPhysical ( data );

	DPMIFreeDosMem ( vbeInfoPool );         // free allocated Dos memory
	DPMIFreeDosMem ( vbeModePool );
	DPMIFreeDosMem ( palPool );

	rmRegs.eax = 3;                         // set mode 3

	RMVideoInt ();
}

int     VESASurface :: findVESAMode ( int mode )
{
	rmRegs.eax = 0x4F01;                    // set up registers for RM interrupt
	rmRegs.ecx = mode;
	rmRegs.es  = vbeModePool.segment;
	rmRegs.edi = 0;

	RMVideoInt ();                          // execute video interrupt

	return ( rmRegs.eax & 0xFFFF ) == 0x4F; // check for validity
}

int     VESASurface :: setVESAMode ( int xRes, int yRes, int bpp )
{
	for ( int i = 0; cardInfo -> modeListPtr [i] != -1; i++ )
	{                                       // retrieve VESAModeInfo
		if ( !findVESAMode ( cardInfo -> modeListPtr [i] ) )
			continue;

		if ( xRes != modeInfo -> xResolution )
			continue;

		if ( yRes != modeInfo -> yResolution )
			continue;

		if ( bpp != modeInfo -> bitsPerPixel )
			continue;
						// check for LFB
		if ( ( modeInfo -> modeAttributes & (1<<7) ) == 0 )
			continue;

		rmRegs.eax = 0x4F02;
		rmRegs.ebx = cardInfo -> modeListPtr [i] | 0x4000;     // ask for LFB

                RMVideoInt ();                  // execute video interrupt

                if ( ( rmRegs.eax & 0xFF ) != 0x4F )
			continue;

                if ( data != NULL )             // unmap previously mapped memory
                        DPMIUnmapPhysical ( data );
                                                // map linear frame buffer
                data = DPMIMapPhysical ( modeInfo -> physBasePtr,
                                (long) cardInfo -> totalMemory * 64 * 1024 );

                format.redBits       = modeInfo -> redMaskSize;
                format.redShift      = modeInfo -> redFieldPosition;
		format.greenBits     = modeInfo -> greenMaskSize;
                format.greenShift    = modeInfo -> greenFieldPosition;
                format.blueBits      = modeInfo -> blueMaskSize;
                format.blueShift     = modeInfo -> blueFieldPosition;
                format.bitsPerPixel  = modeInfo -> bitsPerPixel;
                format.bytesPerPixel = (format.bitsPerPixel + 7) >> 3;
                bytesPerScanLine     = modeInfo -> bytesPerScanLine;
                width                = modeInfo -> xResolution;
                height               = modeInfo -> yResolution;
                rasterOp             = RO_COPY;
		curFont              = NULL;
                clipRect.x1          = 0;
                clipRect.y1          = 0;
                clipRect.x2          = width - 1;
                clipRect.y2          = height - 1;
                org.x                = 0;
                org.y                = 0;

                setFuncPointers ();

		if ( format.bytesPerPixel == 2 )
                {                                       // compute masks from pos's & bit counts
                        format.redMask   = (0xFF >> (8-format.redBits))   << format.redShift;
                        format.greenMask = (0xFF >> (8-format.greenBits)) << format.greenShift;
                        format.blueMask  = (0xFF >> (8-format.blueBits))  << format.blueShift;
                }

                if ( format.bitsPerPixel == 8 ) // paletted mode
                {
                        palette    = new RGB [256];
			rmRegs.eax = 0x1017;    // get palette
                        rmRegs.ebx = 0;         // starting index
                        rmRegs.ecx = 256;       // # of pal entries
                        rmRegs.es  = palPool.segment;
                        rmRegs.edi = 0;

                        RMVideoInt ();          // execute video interrupt

                        RGB * tmpPal = (RGB *) (palPool.segment * 16);

			for ( register int i = 0; i < 256; i++ )
                        {
                                palette [i].red   = (tmpPal [i].red   * 255) / 63;
                                palette [i].green = (tmpPal [i].green * 255) / 63;
                                palette [i].blue  = (tmpPal [i].blue  * 255) / 63;
                        }
                }
                else
                        palette = NULL;

		return 1;
        }

	status = VSS_MODENOTFOUND;

	return 0;
}

char *	VESASurface :: getStatusDescription () const
{
	switch ( status )
	{
		case VSS_OK:
			return "Ok";

		case VSS_VBENOTFOUND:
			return "VBE not found";

		case VSS_WRONGSIGN:
			return "Wrong VESA signature";

		case VSS_WRONGVERSION:
			return "Wrong VBE version";

		case VSS_MODENOTFOUND:
			return "Requested mode not found";

		default:
			return "Unknown error";
	}
}