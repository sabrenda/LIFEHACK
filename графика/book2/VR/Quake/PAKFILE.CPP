#include	<fcntl.h>
#include	<io.h>
#include	<malloc.h>
#include	<string.h>
#include	"pakfile.h"

///////////////////////// BSPFile methods /////////////////////////////////

BSPFile :: BSPFile ()
{
	entities    = NULL;
	planes      = NULL;
	mipTextures = NULL;
	vertices    = NULL;
	visLists    = NULL;
	nodes       = NULL;
	surfaces    = NULL;
	lightMaps   = NULL;
	boundNodes  = NULL;
	leaves      = NULL;
	surfaceList = NULL;
	edges       = NULL;
	edgesList   = NULL;
	hulls       = NULL;
}

BSPFile :: ~BSPFile ()
{
	free ( entities );
	delete [] planes;
	free ( mipTextures );
	delete [] vertices;
	free ( visLists );
	delete [] nodes;
	delete [] surfaces;
	free ( lightMaps );
	delete [] boundNodes;
	delete [] leaves;
	free ( surfaceList );
	delete [] edges;
	free ( edgesList );
	delete [] hulls;
}

int	BSPFile :: isOk () const
{
	if ( entities == NULL || planes == NULL || mipTextures == NULL )
		return 0;

	if ( vertices == NULL || visLists == NULL || nodes == NULL )
		return 0;

	if ( surfaces == NULL || lightMaps == NULL || boundNodes == NULL )
		return 0;

	if ( leaves == NULL || surfaceList == NULL || edges == NULL )
		return 0;

	if ( edgesList == NULL || hulls == NULL )
		return 0;

	return 1;
}

int	BSPFile :: isSurfaceFrontFacing ( Surface& surface ) const
{
	return ( planes [surface.planeNum].normal & loc ) >= 0;
}

int	BSPFile :: isLeafVisible ( int leaf ) const
{
	int	v = curLeaf -> visList;		// start of visibility list for curLeaf

	for ( register int i = 1; i < numLeaves; i++ )
		if ( visLists [v] == 0 )
		{
			i += 8 * visLists [v + 1];
			v++;
		}
		else
		{
			for ( register int bit = 0x80; bit > 0; bit >>= 1, i++ )
				if ( ( i == leaf ) && ( visLists [v] & bit ) )
					return 1;
		}

	return 0;
}

int	BSPFile :: viewerInFront ( Plane& plane ) const
{
	return ( plane.normal & loc ) >= plane.dist;
}

void	BSPFile :: traverseBSPTree ( long node )
{
	Plane * plane = &planes [nodes [node].planeNum];

	if ( node & 0x8000 )		// is it a leaf
	{
		if ( curLeaf == NULL )
			curLeaf = &leaves [~node];

		if ( isLeafVisible ( ~node ) )
			visLeaves.insert ( &leaves [~node] );

		return;
	}
					// check whether node lies
					// in viewing frustrum
	if ( !boxInFrustrum ( nodes [node].boundBox ) )
		return;

	if ( viewerInFront ( *plane ) )
	{
		traverseBSPTree ( nodes [node].front );
		traverseBSPTree ( nodes [node].back  );
	}
	else
	{
		traverseBSPTree ( nodes [node].back  );
		traverseBSPTree ( nodes [node].front );
	}
}

void	BSPFile :: renderLeaf ( BSPLeaf& leaf )
{
	if ( !boxInFrustrum ( leaf.boundBox ) )
		return;

	for ( int i = leaf.firstSurface; i < leaf.firstSurface + leaf.numSurfaces; i++ )
		if ( isSurfaceFrontFacing ( surfaces [surfaceList [i]] ) )
			sBuffer -> addSurface ( surfaces [surfaceList [i]] );
}

void	BSPFile :: render ( Hull& hull )
{
	visLeaves.deleteAll ();
	sBuffer.reset       ();

	curLeaf = NULL;

	traverseBSPTree ( hull.node );

	for ( int i = visLeaves.getCount () - 1; i >= 0; i-- )
		renderLeaf ( *(BSPLeaf *) visLeaves [i] );

	sBuffer.render ();
}

void	BSPFile :: render ()
{
	resetZBuffer ();

	for ( int i = 0; i < numHulls; i++ )
		render ( hulls [i] );
}

///////////////////////////// ModelFile methods /////////////////////////////

ModelFile :: ModelFile ()
{
	skinData  = NULL;
	vertices  = NULL;
	triangles = NULL;
	frames    = NULL;
}

ModelFile :: ~ModelFile ()
{
	free ( skinData );

	delete vertices;
	delete triangles;

	if ( frames != NULL )
		for ( int i = 0; i < hdr.numFrames; i++ )
			free ( frames [i] );

	delete frames;
}

int	ModelFile :: isOk () const
{
	if ( skinData == NULL || vertices == NULL || triangles == NULL || frames == NULL )
		return 0;

	for ( int i = 0; i < hdr.numFrames; i++ )
		if ( frames [i] == NULL )
			return 0;

	return 1;
}

/////////////////////////////// PakFile methods ///////////////////////////

PakFile :: PakFile ( const char * name )
{
	directory = NULL;
	file      = open ( name, O_RDONLY );

	if ( file == -1 )
		return;

	read ( file, &hdr, sizeof ( hdr ) );

	if ( strcmp ( hdr.magic, "PACK" ) )
		return;

	dirEntries = hdr.dirSize / sizeof ( DirEntry );
	directory  = new DirEntry [dirEntries];

	if ( directory == NULL )
		return;

	lseek ( file, hdr.dirOffset, SEEK_SET );
	read  ( file, directory, hdr.dirSize  );
}

PakFile :: ~PakFile ()
{
	if ( directory != NULL )
		delete directory;

	close ( file );
}

int	PakFile :: findFile ( const char * fileName ) const
{
	for ( int i = 0; i < dirEntries; i++ )
		if ( !strcmp ( directory [i].fileName, fileName ) )
			return i;

	return -1;
}

BSPFile   * PakFile :: loadLevel ( const char * levelName )
{
	int	  index = findFile ( levelName );

	if ( index < 0 )
		return NULL;

	BSPFile * bsp   = new BSPFile;

	if ( bsp == NULL )
		return NULL;

	strcpy ( bsp -> fileName, levelName );
	lseek  ( file, directory [index].offset, SEEK_SET );
	read   ( file, &bsp->hdr, sizeof ( BSPHeader ) );

							// load entities
	bsp -> entities = (char *) malloc ( bsp -> hdr.entities.size );

	if ( bsp -> entities == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.entities.offset, SEEK_SET );
	read  ( file, bsp -> entities, bsp -> hdr.entities.size );

							// load planes
	bsp -> numPlanes = bsp -> hdr.planes.size / sizeof ( Plane );
	bsp -> planes    = new Plane [bsp -> numPlanes];

	if ( bsp -> planes == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.planes.offset, SEEK_SET );
	read  ( file, bsp -> planes, bsp -> hdr.planes.size );

							//load textures
	bsp -> mipTextures = malloc ( bsp -> hdr.mipTextures.size );

	if ( bsp -> mipTextures == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.mipTextures.offset, SEEK_SET );
	read  ( file, bsp -> mipTextures, bsp -> hdr.mipTextures.size );

							// load vertices
	bsp -> numVertices = bsp -> hdr.vertices.size / sizeof ( Vector3D );
	bsp -> vertices    = new Vector3D [bsp -> numVertices];

	if ( bsp -> vertices == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.vertices.offset, SEEK_SET );
	read  ( file, bsp -> vertices, bsp -> hdr.vertices.size );

							// load visLists
	bsp -> visLists = (char *) malloc ( bsp -> hdr.visLists.size );

	if ( bsp -> visLists == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.visLists.offset, SEEK_SET );
	read  ( file, bsp -> visLists, bsp -> hdr.visLists.size );

							// load nodes
	bsp -> numNodes = bsp -> hdr.nodes.size / sizeof ( BSPNode );
	bsp -> nodes    = new BSPNode [bsp -> numNodes];

	if ( bsp -> nodes == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.nodes.offset, SEEK_SET );
	read  ( file, bsp -> nodes, bsp -> hdr.nodes.size );

							// load surfaces
	bsp -> numSurfaces = bsp -> hdr.surfaces.size / sizeof ( Surface );
	bsp -> surfaces    = new Surface [bsp -> numSurfaces];

	if ( bsp -> surfaces == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.surfaces.offset, SEEK_SET );
	read  ( file, bsp -> surfaces, bsp -> hdr.surfaces.size );

							// load lighmaps
	bsp -> lightMaps = (char *) malloc ( bsp -> hdr.lightMaps.size );

	if ( bsp -> lightMaps == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.lightMaps.offset, SEEK_SET );
	read  ( file, bsp -> lightMaps, bsp -> hdr.lightMaps.size );

							// load bound nodes
	bsp -> numBoundNodes = bsp -> hdr.boundNodes.size / sizeof ( BoundNode );
	bsp -> boundNodes    = new BoundNode [bsp -> numBoundNodes];

	if ( bsp -> boundNodes == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.boundNodes.offset, SEEK_SET );
	read  ( file, bsp -> boundNodes, bsp -> hdr.boundNodes.size );

							// load leaves
	bsp -> numLeaves = bsp -> hdr.leaves.size / sizeof ( BSPLeaf );
	bsp -> leaves    = new BSPLeaf [bsp -> numLeaves];

	if ( bsp -> leaves == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.leaves.offset, SEEK_SET );
	read  ( file, bsp -> leaves, bsp -> hdr.leaves.size );

							// load surface list
	bsp -> surfaceListSize = bsp -> hdr.surfList.size / sizeof ( short );
	bsp -> surfaceList     = new short [bsp -> surfaceListSize];

	if ( bsp -> surfaceList == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.surfList.offset, SEEK_SET );
	read  ( file, bsp -> surfaceList, bsp -> hdr.surfList.size );

							// load edges
	bsp -> numEdges = bsp -> hdr.edges.size / sizeof ( Edge );
	bsp -> edges    = new Edge [bsp -> numEdges];

	if ( bsp -> edges == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.edges.offset, SEEK_SET );
	read  ( file, bsp -> edges, bsp -> hdr.edges.size );

							// load edge list
	bsp -> edgesListSize = bsp -> hdr.edgeList.size / sizeof ( short );
	bsp -> edgesList     = new short [bsp -> edgesListSize];

	if ( bsp -> edgesList == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.edgeList.offset, SEEK_SET );
	read  ( file, bsp -> edgesList, bsp -> hdr.edgeList.size );

							// load hulls
	bsp -> numHulls = bsp -> hdr.hulls.size / sizeof ( Hull );
	bsp -> hulls    = new Hull [bsp -> numHulls];

	if ( bsp -> hulls == NULL )
		return bsp;

	lseek ( file, directory [index].offset + bsp -> hdr.hulls.offset, SEEK_SET );
	read  ( file, bsp -> hulls, bsp -> hdr.hulls.size );

	return bsp;
}

ModelFile * PakFile :: loadModel ( const char * modelName )
{
	int	    index = findFile ( modelName );

	if ( index < 0 )
		return NULL;

	ModelFile * mdl   = new ModelFile;

	if ( mdl == NULL )
		return NULL;

	strcpy ( mdl -> fileName, modelName );

					// read header
	lseek ( file, directory [index].offset, SEEK_SET );
	read  ( file, &mdl->hdr, 0x4C );

					// load all skins
	mdl -> skinData = (char *) malloc ( mdl -> hdr.numSkins * mdl -> hdr.skinWidth * mdl -> hdr.skinHeight );

	int	skinSize = mdl -> hdr.skinWidth * mdl -> hdr.skinHeight;

	if ( mdl -> skinData == NULL )
		return mdl;			// return invalid object

	for ( int i = 0; i < mdl -> hdr.numSkins; i++ )
	{
		lseek ( file, 4l, SEEK_CUR );
		read  ( file, mdl -> skinData + i * skinSize, skinSize );
	}
					// read skin vertices
	mdl -> vertices = new STVertex [mdl -> hdr.numVertices];

	if ( mdl -> vertices == NULL )
		return mdl;

	read ( file, mdl -> vertices, mdl -> hdr.numVertices * sizeof ( STVertex ) );

					// load triangles
	mdl -> triangles = new Triangle [mdl -> hdr.numTriangles];

	if ( mdl -> triangles == NULL )
		return mdl;

	read ( file, mdl -> triangles, mdl -> hdr.numTriangles * sizeof ( Triangle ) );

					// load frames
	mdl -> frames = new Frame * [mdl -> hdr.numFrames];

	if ( mdl -> frames == NULL )
		return mdl;

	for ( i = 0; i < mdl -> hdr.numFrames; i++ )
	{
		mdl -> frames [i] = (Frame *) malloc ( sizeof ( Frame ) + mdl -> hdr.numTriangles * sizeof ( FrameVertex ) );

		if ( mdl -> frames [i] == NULL )
			return mdl;

		read ( file, mdl -> frames [i], sizeof ( Frame ) + mdl -> hdr.numTriangles * sizeof ( FrameVertex ) );
	}

	return mdl;
}

/////////////////////////////////////////////////////////////////////////////

char  * getExtension ( const char * fileName )
{
	if ( fileName [0] == '\0' )
		return (char *) fileName;

	for ( register int i = strlen ( fileName ) - 1; fileName [i] != '.' && fileName [i] != '/' && i >= 0; i-- )
		;

	if ( i >= 0 && fileName [i] == '.' )
		return (char *)(fileName + i + 1);

	return "";
}
