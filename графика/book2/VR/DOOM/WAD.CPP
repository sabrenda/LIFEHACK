#include        <fcntl.h>
#include        <io.h>
#include        <malloc.h>
#include        <process.h>
#include        <stdio.h>
#include        <string.h>
#include        <sys\stat.h>
#include        "Wad.h"

Vertex  * vertices   = NULL;
LineDef * lines      = NULL;
SideDef * sides      = NULL;
Sector  * sectors    = NULL;
Segment * segs       = NULL;
SSector * subSectors = NULL;
Node    * nodes      = NULL;
Thing   * things     = NULL;
long    * texture1   = NULL;
char    * pnames     = NULL;

RGB       playPal [14][256];
char      colorMap [34][256];

int       numVertices;
int       numLines;
int       numSides;
int       numSectors;
int       numSegs;
int       numSubSectors;
int       numNodes;
int       numThings;
short     numPNames;

/////////////////////////// WadFile methods ////////////////////////////

WadFile :: WadFile ( const char * name )
{
        if ( ( file = open ( strcpy ( fileName, name ), O_RDONLY | O_BINARY ) ) == -1 )
        {
                printf ( "\nCannot open %s.", name );
                exit ( 1 );
        }

        read ( file, &hdr, sizeof ( hdr ) );

        if ( !strnicmp ( hdr.sign, "IWAD", 4 ) && !strnicmp ( hdr.sign, "PWAD", 4 ) )
        {
                printf ( "\nNot a valid WAD file" );
                exit ( 1 );
        }

        directory = new DirEntry [hdr.dirEntries];

        if ( directory == NULL )
        {
                printf ( "\nInsufficint memory to load directory." );
                exit ( 1 );
        }

        lseek ( file, hdr.dirOffset, SEEK_SET );
        read  ( file, directory, hdr.dirEntries * sizeof ( DirEntry ) );
}

WadFile :: ~WadFile ()
{
        close ( file );

        if ( directory != NULL )
                delete directory;
}

void    WadFile :: loadLevel ( int episode, int level )
{
        char    levelName [5] = { 'E', '0' + episode, 'M', '0' + level, '\0' };

        for ( int i = 0; i < hdr.dirEntries; i++ )
                if ( !strcmp ( directory [i].name, levelName ) )
                        break;

        if ( i >= hdr.dirEntries )
                return;

        for ( i++; i < hdr.dirEntries; i++ )
        {
                if ( !strnicmp ( directory [i].name, "VERTEXES", 8 ) )
                {
                        if ( vertices != NULL )
                                delete  vertices;

                        numVertices = directory [i].size / sizeof ( Vertex );
                        vertices    = new Vertex [numVertices];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, vertices, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "LINEDEFS", 8 ) )
                {
                        if ( lines != NULL )
                                delete lines;

                        numLines = directory [i].size / sizeof ( LineDef );
                        lines    = new LineDef [numLines];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, lines, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "SIDEDEFS", 8 ) )
                {
                        if ( sides != NULL )
                                delete sides;

                        numSides = directory [i].size / sizeof ( SideDef );
                        sides    = new SideDef [numSides];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, sides, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "SEGS", 8 ) )
                {
                        if ( segs != NULL )
                                delete segs;

                        numSegs = directory [i].size / sizeof ( Segment );
                        segs    = new Segment [numSegs];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, segs, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "SECTORS", 8 ) )
                {
                        if ( sectors != NULL )
                                delete sectors;

                        numSectors = directory [i].size / sizeof ( Sector );
                        sectors    = new Sector [numSectors];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, sectors, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "SSECTORS", 8 ) )
                {
                        if ( subSectors != NULL )
                                delete subSectors;

                        numSubSectors = directory [i].size / sizeof ( SSector );
                        subSectors    = new SSector [numSubSectors];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, subSectors, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "NODES", 8 ) )
                {
                        if ( nodes != NULL )
                                delete nodes;

                        numNodes = directory [i].size / sizeof ( Node );
                        nodes    = new Node [numNodes];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, nodes, directory [i].size );
                }
                else
                if ( !strnicmp ( directory [i].name, "THINGS", 8 ) )
                {
                        if ( things != NULL )
                                delete things;

                        numThings = directory [i].size / sizeof ( Thing );
                        things    = new Thing [numThings];

                        lseek ( file, directory [i].offset, SEEK_SET );
                        read  ( file, things, directory [i].size );
                }
                else
                        return;
        }
}

void    WadFile :: loadPlayPal ()
{
        int     index = locateResource ( "PLAYPAL" );

        if ( index > -1 )
        {
                lseek ( file, directory [index].offset, SEEK_SET );
                read  ( file, playPal, sizeof ( playPal ) );
        }
}


void    WadFile :: loadColorMap ()
{
        int     index = locateResource ( "COLORMAP" );

        if ( index > -1 )
        {
                lseek ( file, directory [index].offset, SEEK_SET );
                read  ( file, colorMap, sizeof ( colorMap ) );
        }
}

int     WadFile :: locateResource ( const char * name )
{
        for ( register int i = 0; i < hdr.dirEntries; i++ )
                if ( !strnicmp ( directory [i].name, name, 8 ) )
                        return i;

        return -1;
}

Pic   * WadFile :: loadPic ( const char * picName )
{
        int     index = locateResource ( picName );

        if ( index < 0 )                        // not found
                return NULL;
                                                // allocate space for resource
        char      * data   = (char *) malloc ( directory [index].size );
        PicHeader * picHdr = (PicHeader *) data;

        if ( data == NULL )
                return NULL;
                                                // read resource completely
        lseek ( file, directory [index].offset, SEEK_SET );
        read  ( file, data, directory [index].size );

        Pic * pic = new Pic;

        if ( pic == NULL )
        {
                free ( data );

                return NULL;
        }

	pic -> width      = picHdr -> width;
        pic -> height     = picHdr -> height;
        pic -> leftOffset = picHdr -> leftOffset;
        pic -> topOffset  = picHdr -> topOffset;
        pic -> colOffsets = picHdr -> colPtr;
        pic -> data       = data;

        return pic;
}

Texture * WadFile :: loadTexture ( const char * texName )
{
	if ( texture1 == NULL )                 // will use only TEXTURE1
		loadTexture1 ();

	if ( texture1 == NULL )
		return NULL;

	if ( pnames == NULL )
		loadPNames ();

	if ( pnames == NULL )
		return NULL;

	for ( int i = 0; i < texture1 [0]; i++ )
	{
		TextureEntry * entry = (TextureEntry *)( texture1 [i+1] + (char *) texture1 );

		if ( !strnicmp ( entry -> textureName, texName, 8 ) )
		{
			Texture * tex = new Texture;    // init Texture

			if ( tex == NULL )
				return NULL;

			strncpy ( tex -> name, texName, 8 );

			tex -> width  = entry -> width;
			tex -> height = entry -> height;
			tex -> data   = (char *) malloc ( entry -> width * entry -> height );

			if ( tex -> data == NULL )
			{
				delete tex;

				return NULL;
			}

			memset ( tex -> data, 0, entry -> width * entry -> height );
							// apply patches
			for ( int j = 0; j < entry -> numPatches; j++ )
				applyPatch ( tex, entry -> patch [j] );

			return tex;
		}
	}

	return NULL;                                    // texture not found
}

void    WadFile :: applyPatch ( Texture * tex, TexturePatch& patch )
{
	Pic   * pic = loadPic ( pnames + 2 + patch.pnamesNo * 8 );

	if ( pic == NULL )
		return;

	if ( tex -> data == NULL )
		return;

	for ( int col = 0; col < pic -> width; col++ )
	{
		if ( col + patch.xOffset < 0 )
			continue;

		char  * colData  = pic -> data + pic -> colOffsets [col];
		char  * texData;
		int	curRow;

		if ( *colData == '\xFF' )
			continue;

		do
		{
			int     row                  = *colData++;
			int     nonTransparentPixels = *colData++;
			int     count                = nonTransparentPixels;

			curRow = patch.yOffset + row;

			if ( count + curRow >= tex -> height )
				count = tex -> height - 1 - curRow;

			colData++;                      // skip 1st pixel

			texData = tex->data + col + patch.xOffset + curRow * tex -> width;

			for ( register int y = 0; y < count; y++, curRow++ )
			{
				if ( curRow >= 0 )
					*texData = colData [y];

				texData += tex -> width;
			}

			colData += nonTransparentPixels + 1;
		} while ( *colData != '\xFF' );
	}

	freePic ( pic );
}

Texture * WadFile :: loadFloorTexture ( const char * name )
{
	int	index = locateResource ( name );

	if ( index < 0 )
		return NULL;

	Texture * tex = new Texture;

	if ( tex == NULL )
		return NULL;

	strncpy ( tex -> name, name, 8 );

	tex -> width  = 64;
	tex -> height = 64;
	tex -> data   = (char *) malloc ( 4096 );

	if ( tex -> data == NULL )
	{
		delete tex;

		return NULL;
	}

	lseek ( file, directory [index].offset, SEEK_SET );
	read  ( file, tex -> data, 4096 );

	return tex;
}

void    WadFile :: loadTexture1 ()
{
	int     index = locateResource ( "TEXTURE1" );

	texture1 = (long *) malloc ( directory [index].size );

	if ( texture1 == NULL )
		return;

	lseek ( file, directory [index].offset, SEEK_SET );
	read  ( file, texture1, directory [index].size );
}

void    WadFile :: loadPNames ()
{
	int     index = locateResource ( "PNAMES" );

	if ( index < 0 )
		return;

	lseek ( file, directory [index].offset, SEEK_SET );
	read  ( file, &numPNames, sizeof ( numPNames ) );

        pnames = (char *) malloc ( numPNames * 8 );

        if ( pnames == NULL )
		return;

        read ( file, pnames, numPNames * 8 );
}

////////////////////////////////////////////////////////////////////////////

void    freePic ( Pic * pic )
{
        free ( pic -> data );

        delete pic;
}

void    freeTexture ( Texture * tex )
{
        free ( tex -> data );

        delete tex;
}
