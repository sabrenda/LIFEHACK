#ifndef __WAD__
#define __WAD__

#include	"fixmath.h"

///////////////////////// WAD-file structures ///////////////////////////

struct	WadHeader			// header of .WAD file
{
	char	sign [4];		// "IWAD" or "PWAD" signature
	long	dirEntries;		// # of directory entries
	long	dirOffset;		// offset of directory in a file
};

struct	DirEntry			// entry description :
{
	long	offset;			// data offset
	long	size;			// data size
	char	name [8];		// resource name
};

struct	Vertex
{
	short	x;
	short	y;
};

struct	LineDef				// line from one vertex to another
{
	short	fromVertex;
	short	toVertex;
	short	attrs;			// attributes
	short	type;			// type
	short	tag;
	short	sideDefs [2];		// right sidedef, left sidedef ( or -1 )
};

struct	SideDef				// defines wall texture along LineDef
{
	short	xOffset, yOffset;	// texture offsets
	char	upper_tx [8];		// texture names : upper
	char	lower_tx [8];		// lower
	char	main_tx [8];		// main ( regular part if the wall )
	short	sector;			// sector
};

struct	Segment				// SEG, part of linedef, bounding SubSector
{
	short	from, to;
	Angle	angle;			// angle of segment ( in BAM )
	short	lineDef;           	// linedef
	short	lineSide;		// 0 if this seg lies on the right of linedef
					// 1 if on the left
	short	lineOffset;             // offset distance along LineDef to the start of this seg
};

struct	SSector				// SubSector - convex part of Sector
{
	short	numSegs;		// # of segments for this ssector
	short	firstSeg;
};

struct	BBox				// bounding box for BSP trees
{
	short	y2, y1, x1, x2;
};

struct	Node				// BSP node struct
{
	short	x, y;			// from point
	short	dx, dy;			// in direction
	BBox	rightBox;		// bounding box for right subtree
	BBox	leftBox;		// bounding box for left subtree
	short	rightNode, leftNode;	// subtree ( or SSector ) pointers
};

struct	Sector				// area of map with constant heights & textures
{
	short	floorHeight;		// heights:
	short	ceilingHeight;
	char	floor_tx [8];		// textures:
	char	ceiling_tx [8];
	short	light;			// brightness of a sector (0-total dark, 255 - max)
	short	type;
	short	trigger;
};

struct	Thing
{
	short	x, y;
	short	angle;			// in degrees with 45 deg step
	short	type;
	short	attrs;
};

struct	RGB
{
	char	red;
	char	green;
	char	blue;
};

struct	PicHeader                       // internal WAD picture format
{
	short	width;
	short	height;
	short	leftOffset;
	short	topOffset;
	long	colPtr [];		// pointers to PicColumn in WAD file
};

struct	PicColumn
{
	char	row;
	char	nonTransparentPixels;
	char	pixels [];
};

struct	TexturePatch
{
	short	xOffset;
	short	yOffset;
	short	pnamesNo;		// # of this patch in PNAMES resource
	short	one;			// unused, should be 1
	short	zero;			// unsused, should be 0
};

struct	TextureEntry			// in TEXTURE1 and TEXTURE2 resources
{
	char	     textureName [8];
	short	     zero1;		// unused, should always be 0
	short	     zero2;		// unused, should always be 0
	short	     width;
	short	     height;
	short	     zero3;
	short	     zero4;
	short	     numPatches;
	TexturePatch patch [];
};

///////////////////////// non-WAD structures /////////////////////////////

struct	Pic
{
	short	width;
	short	height;
	short	leftOffset;
	short	topOffset;
	long  * colOffsets;
	char  * data;
};

struct	Texture
{
	char	name [8];
	int	width;
	int	height;
	char  * data;
};

///////////////////////// WadFile class ////////////////////////////////////

class	WadFile
{
public:
	WadHeader  hdr;
	DirEntry * directory;
	char	   fileName [128];
	int	   file;

	WadFile ( const char * );
	~WadFile ();

	void	  loadLevel        ( int episode, int level );
	void	  loadPlayPal      ();
	void	  loadColorMap     ();
	Texture * loadTexture      ( const char * name );
	Texture * loadFloorTexture ( const char * name );
	Pic     * loadPic          ( const char * name );
	int	  locateResource   ( const char * name );
	void	  loadTexture1     ();
	void	  loadPNames       ();
	void	  applyPatch       ( Texture * tex, TexturePatch& patch );
};

//////////////////////////////////////////////////////////////////////////

void	freePic     ( Pic * pic );
void	freeTexture ( Texture * tex );

//////////////////////////// global vars /////////////////////////////////

extern	Vertex	* vertices;
extern	LineDef * lines;
extern	SideDef * sides;
extern	Sector  * sectors;
extern	Segment * segs;
extern	SSector * subSectors;
extern	Node	* nodes;
extern	Thing	* things;
extern	long    * texture1;
extern	char    * pnames;
extern	RGB       playPal [14][256];
extern	char	  colorMap [34][256];

extern	int	  numVertices;
extern	int	  numLines;
extern	int	  numSides;
extern	int	  numSectors;
extern	int	  numSegs;
extern	int	  numSubSectors;
extern	int	  numNodes;
extern	int	  numThings;
extern	short	  numPNames;
#endif
