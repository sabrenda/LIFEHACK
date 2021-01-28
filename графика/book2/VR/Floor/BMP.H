#ifndef	__BMP__
#define	__BMP__

#ifndef	__RGB__
#define	__RGB__
struct	RGB
{
	char	red;
	char	green;
	char	blue;
};
#endif

class	BMPImage
{
public:
	int	width;
	int	height;
	char  * data;
	RGB   * palette;

	BMPImage ( char * );
	~BMPImage ();
};
#endif