//
// Class VESASurface for working with VESA SVGA modes
// Author: Alex V. Boreskoff
//
#ifndef	__VESA_SURFACE__
#define	__VESA_SURFACE__

#include	"surface.h"
				// statuses
#define	VSS_OK		 0
#define	VSS_VBENOTFOUND	 1	// VBE not found
#define	VSS_WRONGSIGN	 2	// wrong signature ( not "VESA" )
#define	VSS_WRONGVERSION 3	// VBE version < 2.0
#define	VSS_MODENOTFOUND 4	// requested mode not found

#pragma pack(1)			// byte-align data
struct	VESAInfo
{
	char     vbeSign [4];	// 'VESA' signature
	short    version;	// VESA BIOS version
	char   * OEM;		// Original Equipment Manufactureer string
	long     capabilities;	// capabilities of video card
	short  * modeListPtr;	// list of supported modes
	short	 totalMemory;	// total memory on board in 64Kb blocks
	short    OEMSoftwareRev;// OEM software revision
	char   * OEMVendorName;
	char   * OEMProductName;
	char   * OEMProductRev;
	char	 reserved [222];
	char	 OEMData [256];
};

#pragma pack(1)			// byte-align data
struct	VESAModeInfo
{
	unsigned short	modeAttributes;
	char	        winAAttributes;
	char	        winBAttributes;
	unsigned short	winGranularity;
	unsigned short	winSize;
	unsigned short  winASegment;
	unsigned short  winBSegement;
	void          * winFuncPtr;
	unsigned short  bytesPerScanLine;
	unsigned short  xResolution;
	unsigned short  yResolution;
	char	        xCharSize;
	char	        yCharSize;
	char	        numberOfPlanes;
	char	        bitsPerPixel;
	char	        numberOfBanks;
	char	        memoryModel;
	char	        bankSize;
	char	        numberOfPages;
	char	        reserved;
				// direct color fields
	char	        redMaskSize;
	char	        redFieldPosition;
	char	        greenMaskSize;
	char	        greenFieldPosition;
	char	        blueMaskSize;
	char	        blueFieldPosition;
	char	        rsvdMaskSize;
	char	        rsvdFieldPosition;
	char	        directColorModeInfo;
	void          * physBasePtr;
	void          * offScreenMemoOffset;
	unsigned short  offScreenMemSize;
	char	        resererved2 [206];
};

class	VESASurface : public Surface
{
public:
	VESAInfo     * cardInfo;
	VESAModeInfo * modeInfo;

	int	initVBE2 ();
	void	doneVBE2 ();


	VESASurface ( int xRes, int yRes, int bpp );
	virtual ~VESASurface ();

	virtual	char *	getStatusDescription () const;

	int	findVESAMode ( int mode );
	int	setVESAMode  ( int xRes, int yRes, int bpp );
	int	getVESAMode  ();
};

#endif
