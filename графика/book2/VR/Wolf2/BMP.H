#ifndef	__BMP__
#define	__BMP__

#ifndef	__RGB__
#define	__RGB__
struct	RGB
{
	char	Red;
	char	Green;
	char	Blue;
};
#endif

class	Image
{
public:
	int	Width, Height;
	char  * Data;

	virtual	~Image () {};

	virtual	char	GetPixel ( int, int ) = 0;
};

class	BMPImage : public Image
{
public:
	RGB	* Palette;

	BMPImage ( char * );
	~BMPImage ();

	virtual	char	GetPixel ( int, int );
};
#endif