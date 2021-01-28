#ifndef	__PAK_FILE__
#define	__PAK_FILE__

#include	"array.h"
#include	"vector3D.h"

struct	PakHeader
{
	char	magic [4];		// "PACK"
	long	dirOffset;		// position of a directory in the file
	long	dirSize;		// # of entries * 0x40
};

struct	DirEntry
{
	char	fileName [0x38];	// name of file, Unix style withe ext
	long	offset;			// position of entry data in the file
	long	size;			// size of data
};

///////////////////////////// BSP structures //////////////////////////////

struct	BSPDirEntry
{
	long	offset;
	long	size;
};

struct	BSPHeader			// header of the .BSP file (lump)
{
	long	    version;		// must be 0x17
	BSPDirEntry entities;		// list of entities
	BSPDirEntry planes;		// map planes
	BSPDirEntry mipTextures;	// wall textures
	BSPDirEntry vertices;		// map vertices
	BSPDirEntry visLists;		// visibility lists
	BSPDirEntry nodes;		// BSP nodes
	BSPDirEntry surfaces;		// map surfaces
	BSPDirEntry lightMaps;
	BSPDirEntry boundNodes;		// BSP bound nodes, for Hulls
	BSPDirEntry leaves;		// BSP leaves
	BSPDirEntry surfList;		// list of surfaces
	BSPDirEntry edges;		// original surface edges
	BSPDirEntry edgeList;		// list of edges
	BSPDirEntry hulls;		// list of hulls
};

class	BoundBox
{
public:
	Vector3D min;
	Vector3D max;

	BoundBox () {}
};

class	Hull
{
public:
	BoundBox bound;			// bounding box of the hull
	long	 zero [3];		// alway zero
	long	 node;			// index of 1st BSP node
	long	 boundNode;		// index of 1st bound BSP node
	long	 numLeafs;		// number of BSP leaves
	long	 firstSurface;		// id of surfaces
	long	 numSurfaces;		// number of surfaces

	Hull () {}
};

struct	Edge
{
	short	startVertex;		// index of the 1st vertex
	short	endVertex;		// index of the last vertex
};

struct	Surface
{
	short	planeNum;		// the plane in which the surface lies
	short	behindPlane;		// 1 if the behind the plane
	char	texNum;			// id of mip texture
	char	sOffs;			// horzontal offset in texture space
	char	tOffs;			// vertical offset in texture space
	char	flips;			// if bit 0 == 1 flip vertically
					// if bit 1 == 1 flip horizontally
					// if bit 2 == 1 exchnge vertical and horizontal coordinates
	long	firstEdge;		// 1st edge in the list of edges
	long	numEdges;		// number of edges in the list of edges
	char	lightType;
	char	baseLight;		// from 0xFF (dark) to 0 (bright)
	short	unknown;		// 0xFFFF
	long	lightMap;		// pointer inside general light map or -1
};

struct	MipTextureHeader
{
	long	numTextures;		// number of textures in Mip Texture list
	long	offset [];		// numTextures offsets from the start of mip header
};

struct	MipTexture
{
	char	name [16];
	long	width;
	long	height;
	long	offset1;
	long	offset2;
	long	offset4;
	long	offset8;
};

class	BSPNode
{
public:
	long	 planeNum;		// plane that splits the node
	short	 front;			// if bit 15 == 0, index of front child node
					// if bit 15 == 1, ~front = index of child leaf
	short	 back;			// if bit 15 == 0, index of back child node
					// if bit 15 == 1, ~front = index of child leaf
	BoundBox boundBox;		// bounding box for node and all childs

	BSPNode () {}
};

class	BSPLeaf
{
public:
	long	 leafType;
	BoundBox boundBox;
	long	 visList;		// start of visibility lists or -1
	long	 firstSurface;		// 1st item of the list of surfaces
	long	 numSurfaces;		// number of surfaces in the list
	long	 zeroes [3];		// always 0
	short	 zero;			// always 0
	short	 flag;			// always 0xFFFF

	BSPLeaf () {}
};

class	Plane
{
public:
	Vector3D normal;
	float	 dist;
	long	 type;			// type of plane, depending on normal vector
					// 0: facing towards X   3: towards -X
					// 1: facing towards Y   4: towards -Y
					// 2: facing towards Z   5: towards -Z
	long	firstSurface;		// index of the 1st surface in that plane
	int	numSurfaces;		// number of consecutive surfaces in the plane

	Plane () {}
};

struct	BoundNode
{
	long	planeNum;
	short	front;			// if positive, id of front child node
					// if -2, front part is inside the hull
					// if -1, front part is outside the hull
	short	back;			// if positive, id of back child node
					// if -2, back part is inside the hull
					// if -1, back part is outside the hull
};

class	BSPFile
{
public:
	char	    fileName [0x38];
	BSPHeader   hdr;
	char	  * entities;		// list of entities
	long	    entitiesSize;	// size of entities in bytes
	Plane     * planes;		// list of planes
	int	    numPlanes;		// number of planes
	void	  * mipTextures;	// mip textures file
	Vector3D  * vertices;
	int	    numVertices;
	char	  * visLists;		// visibility lists
	BSPNode   * nodes;
	int	    numNodes;
	Surface   * surfaces;
	int    	    numSurfaces;
	char      * lightMaps;
	BoundNode * boundNodes;
	int	    numBoundNodes;
	BSPLeaf   * leaves;
	int	    numLeaves;
	short	  * surfaceList;
	int	    surfaceListSize;
	Edge      * edges;
	int	    numEdges;
	short	  * edgesList;
	int	    edgesListSize;
	Hull      * hulls;
	int	    numHulls;

	Array	    visLeaves;
	BSPLeaf   * curLeaf;

	BSPFile  ();
	~BSPFile ();

        int	isOk                 () const;
	int	isLeafVisible        ( int leaf ) const;
	int	boxInFrustrum        ( BoundBox& box ) const;
	int	isSurfaceFrontFacing ( Surface& ) const;
	int	viewerInFront        ( Plane&  ) const;
	void	traverseBSPTree      ( long node );
	void	renderLeaf           ( BSPLeaf& leaf );
	void	render               ( Hull&  hull );
	void	render               ();
};

///////////////////////////// MDL structures ////////////////////////////////

class	MDLFileHeader
{
public:
	long	 id;			// 0x4F504449 = "IDPO"
	long	 version;		// Version = 3
	Vector3D scale;			// scale factor
	Vector3D origin;		// model origin
	float	 radius;		// model radius
	Vector3D offsets;
	long	 numSkins;		// number of skin textures
	long	 skinWidth;		// width of skin texture, must be a multiple of 8
	long	 skinHeight;		// height of skin texture, must be a multile of 8
	long	 numVertices;		// number of vertices
	long	 numTriangles;		// number of triangles
	long	 numFrames;		// number of frames
	long	 zero;			// 0

	MDLFileHeader ();
};

struct	STVertex
{
	long	onSeam;			// 0 or 1
	long	s;
	long	t;
};

struct	Triangle
{
	long	onFrontSkin;
	long	vertices [3];
};

struct	FrameVertex
{
	char	packedPosition [3];	// X, Y, Z coordinates, packed on 0-255
					// to get real coordinates multiply by scaling factor
	char	lightNormalIndex;	// index of the vertex normal into the precalculated normal vector
};

struct	Frame
{
	long	    zero;		// 0
	FrameVertex min;		// minimum values of X,Y,Z
	FrameVertex max;		// maximum values of X,Y,Z
	FrameVertex frame [];		// array with numVertices elements
};

class	ModelFile
{
public:
	char	       fileName [0x38];
	MDLFileHeader  hdr;
	char	     * skinData;	// data for all skins
	STVertex     * vertices;	// texture indices for every vertex
	Triangle     * triangles;
	Frame       ** frames;

	ModelFile ();
	~ModelFile ();

	int	isOk () const;
};

/////////////////////////////////////////////////////////////////////////////

class	PakFile
{
	int	   file;
	PakHeader  hdr;
	int	   dirEntries;
	DirEntry * directory;
public:
	PakFile ( const char * name );
	~PakFile ();

	int	    isOK () const
	{
		return file != -1 && directory != NULL;
	}

	int	    findFile ( const char * fileName ) const;
	BSPFile   * loadLevel ( const char * name );
	ModelFile * loadModel ( const char * name );
};

extern	Vector3D	loc;			// location a viewer

char * getExtension ( const char * fileName );

#endif
