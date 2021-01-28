#include        <i86.h>
#include        <malloc.h>
#include        <string.h>
#include        "vesa.h"

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
static  REGS           regs;
static  SREGS          sregs;

VESAInfo     * vbeInfoPtr;
VESAModeInfo * vbeModeInfoPtr;
void         * lfbPtr = NULL;
int            bytesPerLine;
int            bitsPerPixel;

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
        regs.w.ax  = 0x0300;
        regs.w.bx  = 0x0010;
        regs.w.cx  = 0;
        regs.x.edi = FP_OFF ( &rmRegs );
        sregs.es   = FP_SEG ( &rmRegs );

        int386x ( 0x31, &regs, &regs, &sregs );
}

initVBE2 ()
{
        memset ( &regs,  '\0', sizeof ( regs ) );
        memset ( &sregs, '\0', sizeof ( sregs ) );
        memset ( &rmRegs, '\0', sizeof ( rmRegs ) );

        DPMIAllocDosMem ( vbeInfoPool, 512 / 16 );
        DPMIAllocDosMem ( vbeModePool, 256 / 16 );

        vbeInfoPtr     = (VESAInfo *)( vbeInfoPool.segment * 16 );
        vbeModeInfoPtr = (VESAModeInfo *)( vbeModePool.segment * 16 );

        memset  ( vbeInfoPtr, '\0', sizeof ( VESAInfo ) );
        strncpy ( vbeInfoPtr -> vbeSign, "VBE2", 4 );

        rmRegs.eax = 0x4F00;
        rmRegs.es  = vbeInfoPool.segment;
        rmRegs.edi = 0;

        RMVideoInt ();

        if ( rmRegs.eax != 0x4F )
                return 0;

        vbeInfoPtr -> OEM            = (char *)  RM2PMPtr ( vbeInfoPtr -> OEM );
        vbeInfoPtr -> modeListPtr    = (short *) RM2PMPtr ( vbeInfoPtr -> modeListPtr );
        vbeInfoPtr -> OEMVendorName  = (char *)  RM2PMPtr ( vbeInfoPtr -> OEMVendorName );
        vbeInfoPtr -> OEMProductName = (char *)  RM2PMPtr ( vbeInfoPtr -> OEMProductName );
        vbeInfoPtr -> OEMProductRev  = (char *)  RM2PMPtr ( vbeInfoPtr -> OEMProductRev );

        if ( strncmp ( vbeInfoPtr -> vbeSign, "VESA", 4 ) )
                return 0;

        if ( vbeInfoPtr -> version >= 0x0200 && vbeInfoPtr -> OEMVendorName == NULL )
                vbeInfoPtr -> version = 0x0102;

        return vbeInfoPtr -> version >= 0x200;
}

void    doneVBE2 ()
{
        if ( lfbPtr != NULL )                   // unmap mapped LFB
                DPMIUnmapPhysical ( lfbPtr );

        DPMIFreeDosMem ( vbeInfoPool );         // free allocated Dos memory
        DPMIFreeDosMem ( vbeModePool );

        setVESAMode ( 3 );
}

findVESAMode ( int mode )
{
        rmRegs.eax = 0x4F01;                    // set up registers for RM interrupt
        rmRegs.ecx = mode;
        rmRegs.es  = vbeModePool.segment;
        rmRegs.edi = 0;

        RMVideoInt ();                          // execute video interrupt

        return ( rmRegs.eax & 0xFFFF ) == 0x4F; // check for validity
}

setVESAMode ( int mode )
{
        if ( mode == 3 )
        {
                rmRegs.eax = 3;

                RMVideoInt ();

                return 1;
        }

        if ( !findVESAMode ( mode ) )           // retrieve VESAModeInfo
                return 0;

        rmRegs.eax = 0x4F02;
        rmRegs.ebx = mode | 0x4000;             // ask foir LFB

        RMVideoInt ();                          // execute video interrupt

        if ( ( rmRegs.eax & 0xFFFF ) != 0x4F )
                return 0;

        if ( lfbPtr != NULL )                   // unmap previously mapped memory
                DPMIUnmapPhysical ( lfbPtr );
                                                // map linear frame buffer
        lfbPtr = DPMIMapPhysical ( vbeModeInfoPtr -> physBasePtr,
                       (long) vbeInfoPtr -> totalMemory * 64 * 1024 );

        bytesPerLine = vbeModeInfoPtr -> bytesPerScanLine;
        bitsPerPixel = vbeModeInfoPtr -> bitsPerPixel;

        return 1;
}
