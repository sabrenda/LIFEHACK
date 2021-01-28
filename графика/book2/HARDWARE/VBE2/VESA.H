#ifndef	__VESA__
#define	__VESA__
					// 256-color modes
#define	VESA_640x400x256	0x100
#define	VESA_640x480x256	0x101
#define	VESA_800x600x256	0x103
#define	VESA_1024x768x256	0x105
#define	VESA_1280x1024x256	0x107
					// 32K color modes
#define	VESA_320x200x32K	0x10D
#define	VESA_640x480x32K	0x110
#define	VESA_800x600x32K	0x113
#define	VESA_1024x768x32K	0x116
#define	VESA_1280x1024x32K	0x119
					// 64K color modes
#define	VESA_320x200x64K	0x10E
#define	VESA_640x480x64K	0x111
#define	VESA_800x600x64K	0x114
#define	VESA_1024x768x64K	0x117
#define	VESA_1280x1024x64K	0x11A
					// 16M color mode
#define	VESA_320x200x16M	0x10F
#define	VESA_640x480x16M	0x112
#define	VESA_800x600x16M	0x115
#define	VESA_1024x768x16M	0x118
#define	VESA_1280x1024x16M 	0x11B

struct	VESAInfo
{
	char     vbeSign [4];	// 'VESA' signature
	short    version;	// VESA BIOS version
	char   * OEM;		// Original Equipment Manufactureer string
	long     capabilities;
	short  * modeListPtr;	// list of supported modes
	short	 totalMemory;	// total memory on board in 64Kb blocks
	short    OEMSoftwareRev;// OEM software revision
	char   * OEMVendorName;
	char   * OEMProductName;
	char   * OEMProductRev;
	char	 reserved [222];
	char	 OEMData [256];
};

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

extern	VESAInfo     * vbeInfoPtr;
extern	VESAModeInfo * vbeModeInfoPtr;
extern	void         * lfbPtr;
extern	int	       bytesPerLine;
extern	int	       bitsPerPixel;

int	initVBE2 ();
void	doneVBE2 ();
int	findVESAMode ( int );
int	setVESAMode ( int );
int	getVESAMode ();
#endif
