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
	char	   sign [4];	// 'VESA' signature
	int	   version;	// VESA BIOS version
	char far * OEM;		// Original Equipment Manufactureer string
	long	   capabilities;
	int far  * modeList;	// list of supported modes
	int	   totalMemory;	// total memory on board in 64Kb blocks
	char	   reserved [236];
};

struct	VESAModeInfo
{
	int	   modeAttributes;
	char	   winAAttributes;
	char	   winBAttributes;
	int	   winGranularity;
	int	   winSize;
	unsigned   winASegment;
	unsigned   winBSegement;
	void far * winFuncPtr;
	int	   bytesPerScanLine;
				// optional data
	int	   xResolution;
	int	   yResolution;
	char	   xCharSize;
	char	   yCharSize;
	char	   numberOfPlanes;
	char	   bitsPerPixel;
	char	   numberOfBanks;
	char	   memoryModel;
	char	   bankSize;
	char	   numberOfPages;
	char	   reserved;
				// direct color fields
	char	   redMaskSize;
	char	   redFieldPosition;
	char	   greenMaskSize;
	char	   greenFieldPosition;
	char	   blueMaskSize;
	char	   blueFieldPosition;
	char	   rsvdMaskSize;
	char	   rsvdFieldPosition;
	char	   directColorModeInfo;
	char	   resererved2 [216];
};

int	findVESA ( VESAInfo& );
int	findVESAMode ( int, VESAModeInfo& );
int	setVESAMode ( int );
int	getVESAMode ();
void	setVESABank ();
#endif
