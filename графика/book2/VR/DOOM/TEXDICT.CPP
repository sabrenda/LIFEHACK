#include        "texDict.h"

int     texCompare ( const void * tex1, const void * tex2 )
{
        return strnicmp ( ((Texture *) tex1) -> name, ((Texture *) tex2) -> name, 8 );
}

///////////////////////// TextureDictionary methods //////////////////////

TextureDictionary :: TextureDictionary ( WadFile * wad )
       : Dictionary ( 700, 50, texCompare )
{
        wadFile = wad;
}

Texture * TextureDictionary :: getTexture ( const char * name )
{
        if ( name [0] == '-' )
                return NULL;

        int     index;

	if ( find ( (const void *) name, index ) )
		return (Texture *) (*this) [index];

	Texture * tex = wadFile -> loadTexture ( name );

	if ( tex == NULL )
		return NULL;

	insert ( tex );

	return tex;
}

