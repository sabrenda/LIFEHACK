#ifndef	__TEXTURE_DICTIONARY__
#define	__TEXTURE_DICTIONARY__

#include	"dict.h"
#include	"wad.h"

class	TextureDictionary : public Dictionary
{
	WadFile	* wadFile;
public:
	TextureDictionary ( WadFile * wad );

	virtual	void	freeItem ( void * item )
	{
		freeTexture ( (Texture *) item );
	}

	Texture * getTexture ( const char * name );
};

#endif
